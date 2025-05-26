# %%
import os
import re
from collections.abc import MutableMapping
from datetime import datetime, time
from pprint import pprint

import keras_tuner as kt
import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns
import tensorflow as tf
import yaml
from IPython.display import Markdown, display
from livelossplot import PlotLossesKeras
from sklearn.model_selection import train_test_split
from tensorflow import keras
from tensorflow.keras import callbacks, layers, models

# %% [md]
"""
# Загрузка данных
Загрузим стандартные значения из атласа МЭИ.
К числу стандартных отнесем следующие параметры:
- $\vec{t}$ - стандартный относительный шаг;
- $\vec{b}$ - стандартная хорда;
- $\alpha_y \text{ or } \beta_y$ - угол установки;
- $r_{in}$ - радиус входной кромки;
- $r_{out}$ - радиус выходной кромки;
- $\vec{t}_{опт}$ - диапазон относительного оптимального шага;
- $\alpha_0 \text{ or } \beta_1$ - диапазон угла входа потока;
- $\alpha_1 \text{ or } \beta_2$ - диапазон угла выхода потока;
- $M$ -  диапазон числа Mаха;
"""

# %%
GridsFilePath = "/mnt/A/REMOTE/MPEI/DIP/part 2/split/API/grids.xlsx"
grids = pd.read_excel(GridsFilePath)
grids = grids[grids["isOn"] == 1].drop(
    columns=[
        "isOn",
        "center_X",
        "center_Y",
        "F",
        "a",
        "Ixx",
        "Iyy",
        "Wxx_back",
        "Wxx_edge",
        "Wyy_in_edge",
        "Wyy_out_edge",
        "B",
    ]
)

combined_cols = {}
for col in grids.columns:
    if col.endswith("_b"):
        prefix = col[:-2]
        b_col = col
        e_col = prefix + "_e"
        if e_col in grids.columns:
            combined_cols[prefix] = (b_col, e_col)

# Step 2: Create new columns with tuple values
for prefix, (b_col, e_col) in combined_cols.items():
    grids[prefix] = list(zip(grids[b_col], grids[e_col]))
    grids.drop(columns=[b_col, e_col], inplace=True)

grids
# %% [md]
"""
Так как разработанная программа на данном этапе никак не связана с углом установки и относительным шагом, то эти параметры учитываться не будут. Это обстоятельство обусловенно фактом, что угол установки и относительный шаг являются параметрами канала, а не профиля.
"""
# %%
grids = grids.drop(columns=["rel_t", "ins_angle", "ins_angle_expression", "t_opt"])
grids.set_index("Name", inplace=True)
grids.index.name = None
grids

# %% [md]
"""
Теперь загрузим данные, полученные с помощью разработаннной программы. Данные хранятся в файлах с расширением `.yaml`
"""


# %%
def readSingleFile(path):
    with open(path, "r") as file:
        data = yaml.safe_load(file)
        return data


def filesList(dir):
    list = []
    norm = os.path.normpath(dir)
    for elem in os.listdir(norm):
        split = os.path.splitext(elem)
        if split[1] == ".yaml":
            list.append(os.path.join(dir, elem))
    return list


# %%
files = filesList("/home/snj/Data/Sections/")
files
# %%

bffiles = {}

# Parsing grid name
for file in files:
    name = os.path.basename(file)
    name, ext = os.path.splitext(name)
    atlasName = name.replace("S", "С")
    atlasName = atlasName.replace("R", "Р")
    atlasName = atlasName.replace("A", "А")
    atlasName = atlasName.replace("B", "Б")
    bffiles[atlasName] = {"filename": name, "path": file}
bfdf = pd.DataFrame(bffiles).T
bfdf

# %% [md]
"""
Теперь объединим данные
"""
# %%
preAllGridsDf = pd.merge(grids, bfdf, left_index=True, right_index=True, how="left")
preAllGridsDf
# %% [md]
"""
Видно, что некоторые решетки из собранного ранее (бакалавриат) были собранны из нескольких источников, в число которых входил не только атлас МЭИ. Для каждого профиля из атласа МЭИ были спроектированы сечения, поэтому, если в таблице выше есть `NaN`-значения - их необходимо исключить.
"""
# %%
preNeededGridsDf = preAllGridsDf.dropna()
preNeededGridsDf
# %% [md]
"""
# Обзор формата BladeForge
Обратимся к основным параметрам, которые хранятся в `.yaml` файлах. Для наглядности прочитаем файл `/home/snj/Data/Sections/R-23-14A.yaml`:
"""


# %%
def showYAML(data):
    pretty_yaml = yaml.dump(data, sort_keys=False, indent=4)
    display(Markdown(f"```yaml\n{pretty_yaml}\n```"))


with open("/home/snj/Data/Sections/R-23-14A.yaml", "r") as f:
    data = yaml.safe_load(f)
    showYAML(data)

# %% [md]
"""
Некоторые ноды и поля для данной задачи требоваться не будут, поэтому они будут вырезаны. К числу таких полей относится `imageData` и `color`, `norm` для каждой из записанных точек.
Определим функцию для упрощения читаемости формата:
"""


# %%
def readAndProcessYAML(path):
    def removeNodeWithName(name, obj):
        """Remove nodes with name=name from input dict"""
        if isinstance(obj, dict):
            if name in obj:
                del obj[name]
            for k in list(obj):
                removeNodeWithName(name, obj[k])
        elif isinstance(obj, list):
            for item in obj:
                removeNodeWithName(name, item)

    with open(path, "r") as f:
        data = yaml.safe_load(f)
        # Цвета точек
        removeNodeWithName("color", data)
        # Нормали точек
        removeNodeWithName("norm", data)
        # Данные о изображении (необходимые для загрузки модели в bf)
        removeNodeWithName("imageData", data)
        # Internal bf data
        removeNodeWithName("renderBitSet", data)
        return data


# %%
testYAML = readAndProcessYAML("/home/snj/Data/Sections/R-23-14A.yaml")
showYAML(testYAML)

# %% [md]
"""
Проектирование профилей было выполнено с соблюдением одинаковых размерностей данных. Изначально в `bf` можно использовать неограниченное количество управляющих окружностей [УО] и степень кривой Безье для начальной кривой (< 10), где для каждой степени + 1 нужно хранить свои точки. Такой подход не может быть использован, если необходимо обучить нейронную сеть, масштаб данных которой всегда фиксирован.

Так как все `.yaml` файлы имеют одинаковую размерность, преобразуем их в одномерный вид и соберем из них таблицу.

Так же уберем `z` координату, так как она для всех точек одинакова.
"""


# %%
def flattenYAML(d, parent_key="", sep="."):
    items = {}
    for k, v in d.items():
        new_key = f"{parent_key}{sep}{k}" if parent_key else k
        if isinstance(v, dict):
            items.update(flattenYAML(v, new_key, sep=sep))
        elif isinstance(v, list):
            for i, item in enumerate(v):
                if isinstance(item, dict):
                    # flatten dicts inside lists
                    items.update(flattenYAML(item, f"{new_key}[{i}]", sep=sep))
                else:
                    # save arrays as-is (or convert to str)
                    items[f"{new_key}[{i}]"] = item
        else:
            items[new_key] = v
    return items


# %%
flattenTestYAML = flattenYAML(testYAML)
showYAML(flattenTestYAML)

# %% [md]
"""
Теперь дропнем все `z` координаты
"""


# %%
def eraseZinYAML(data):
    new = {}
    for k, d in data.items():
        if not k.endswith(".pos[2]"):
            new[k] = d
    return new


# %%
erasedFlattenTestYAML = eraseZinYAML(flattenTestYAML)
showYAML(erasedFlattenTestYAML)

# %% [md]
"""
Загрузим об объединим все спроектированные сечения
"""
# %%
yaml_expanded = preNeededGridsDf["path"].apply(
    lambda path: eraseZinYAML(flattenYAML(readAndProcessYAML(path)))
)
yaml_expanded_df = pd.DataFrame(yaml_expanded.tolist())
yaml_expanded_df = yaml_expanded_df.add_prefix("YAML.")
preBfDf = pd.concat(
    [preNeededGridsDf.reset_index(drop=True), yaml_expanded_df.reset_index(drop=True)],
    axis=1,
).set_index(preNeededGridsDf.index)
preBfDf.T
# %% [md]
"""
Из преобразованных данных выше видно, что, некоторые поля повторяются. К примеру, `YAML.*Chord.*`, которое действительно не менялось при проектировании профилей. 

Уберем так же поля `YAML.chord` `YAML.step` `YAML.installAngle` `YAML.z`, так как они либо одинаковые либо не имеют отношения к дальнейшим действиям, о чем было упомянуто ранее.
"""


# %%
def findColsWithRegex(df, regex):
    """
    Return a list of column names in the DataFrame `df` that match the regex pattern `regex`.
    """
    pattern = re.compile(regex)
    return [col for col in df.columns if pattern.search(col)]


# %%
colsToErase = [
    *findColsWithRegex(preBfDf, "YAML.*Chord.*"),
    "YAML.z",
    "YAML.chord",
    "YAML.step",
    "YAML.installAngle",
    "YAML.initialBezierCurveOrder",
    "filename",
    "path",
]
colsToErase

# %%
preErasedBfDf = preBfDf.drop(columns=colsToErase)
preErasedBfDf.T

# %% [md]
"""
Разберем обратно столбцы с диапазонами
"""


# %%
def split_tuple_columns(df):
    new_cols = {}
    for col in df.columns:
        if df[col].apply(lambda x: isinstance(x, tuple)).all():
            new_cols[f"{col}_LEFT"] = df[col].apply(lambda x: x[0])
            new_cols[f"{col}_RIGHT"] = df[col].apply(lambda x: x[1])
        else:
            new_cols[col] = df[col]
    return pd.DataFrame(new_cols)


# %%
bfDf = split_tuple_columns(preErasedBfDf)
bfDf.T

# %% [md]
"""
Обратим внимание на приведенные данные
"""
# %%
bfDf.describe().T
# %%
angleCols = findColsWithRegex(bfDf, ".*[aA]ngle.*")

maxAngle = 120
minAngle = 0


def makeVioPlot(data, title):
    plt.figure(figsize=(20, 8))
    sns.violinplot(data=data)
    plt.xticks(rotation=45)
    plt.title("Angle-related Columns Violin Plot", fontsize=16)
    plt.yticks(fontsize=12)
    plt.setp(plt.gca().get_xticklabels(), ha="right", fontsize=12)
    plt.tight_layout()
    plt.title(title)
    plt.axhline(maxAngle)
    plt.axhline(minAngle)
    plt.show()


def makeBoxPlot(data, title):
    plt.figure(figsize=(20, 8))
    sns.boxplot(data=data)
    plt.xticks(rotation=45)
    plt.title(title, fontsize=16)
    plt.yticks(fontsize=12)
    plt.setp(plt.gca().get_xticklabels(), ha="right", fontsize=12)
    plt.tight_layout()
    plt.axhline(maxAngle)
    plt.axhline(minAngle)
    plt.show()


# %% fmt: off
title = "Распределение углов для сечений обоих типов (vio)"
makeVioPlot(bfDf[angleCols], title)
title = "Распределение углов для сечений обоих типов (box)"
makeBoxPlot(bfDf[angleCols], title)

# %%
title = "Распределение углов для сечений сопловых лопаток"
makeVioPlot(bfDf[bfDf["type"] == 0][angleCols], title)
title = "Распределение углов для сечений сопловых лопаток"
makeBoxPlot(bfDf[bfDf["type"] == 0][angleCols], title)

# %%
title = "Распределение углов для сечений рабочих лопаток"
makeVioPlot(bfDf[bfDf["type"] == 1][angleCols], title)
title = "Распределение углов для сечений рабочих лопаток"
makeBoxPlot(bfDf[bfDf["type"] == 1][angleCols], title)

# %% [md]
"""
Из графиков выше видно, что распределение углов в целом укладывается в диапазон 0-150 градусов. 
Нормализуем так же радиусы входа и выхода, поделив их не длину хорды
"""

# %%
initialAngleCols = findColsWithRegex(bfDf, ".*[aA]ngle.*")
bfDfNorm = bfDf.copy(True)
for angle in initialAngleCols:
    bfDfNorm[f"{angle}_NORM"] = (bfDfNorm[angle] - minAngle) / (maxAngle - minAngle)
bfDfNorm[f"Delta_in_NORM"] = bfDfNorm["Delta_in"] / bfDfNorm["b"]
bfDfNorm[f"Delta_out_NORM"] = bfDfNorm["Delta_out"] / bfDfNorm["b"]
bfDfNorm.columns

# %% [md]
"""
# Обучение модели
"""
# %%
# Assuming your DataFrame is bfDfNorm
# 1. Input columns: all columns *not* starting with 'YAML.'
input_cols = [col for col in bfDfNorm.columns if not col.startswith("YAML.")]
input_cols = [
    "type",
    # "b",
    # "Delta_in",
    # "Delta_out",
    # "out_angle_LEFT",
    # "out_angle_RIGHT",
    # "in_angle_LEFT",
    # "in_angle_RIGHT",
    "M_LEFT",
    "M_RIGHT",
    "out_angle_LEFT_NORM",
    "out_angle_RIGHT_NORM",
    "in_angle_LEFT_NORM",
    "in_angle_RIGHT_NORM",
    "Delta_in_NORM",
    "Delta_out_NORM",
]
# pprint(input_cols)
# 2. Output columns: normalized YAML columns only, i.e. columns starting with 'YAML.' AND ending with '_NORM'
output_cols = [col for col in bfDfNorm.columns if col.startswith("YAML.")]
output_cols = [
    "YAML.inletRadius",
    "YAML.outletRadius",
    "YAML.centerCircles[0].relativePos",
    "YAML.centerCircles[0].radius",
    "YAML.centerCircles[1].relativePos",
    "YAML.centerCircles[1].radius",
    "YAML.centerCircles[2].relativePos",
    "YAML.centerCircles[2].radius",
    "YAML.initialCurveControlVertices[0].pos[0]",
    "YAML.initialCurveControlVertices[0].pos[1]",
    "YAML.initialCurveControlVertices[1].pos[0]",
    "YAML.initialCurveControlVertices[1].pos[1]",
    "YAML.initialCurveControlVertices[2].pos[0]",
    "YAML.initialCurveControlVertices[2].pos[1]",
    "YAML.initialCurveControlVertices[3].pos[0]",
    "YAML.initialCurveControlVertices[3].pos[1]",
    "YAML.initialCurveControlVertices[4].pos[0]",
    "YAML.initialCurveControlVertices[4].pos[1]",
    "YAML.initialCurveControlVertices[5].pos[0]",
    "YAML.initialCurveControlVertices[5].pos[1]",
    "YAML.inletAngle_NORM",
    "YAML.outletAngle_NORM",
    "YAML.inletBackVertexAngle_NORM",
    "YAML.inletFrontVertexAngle_NORM",
    "YAML.outletBackVertexAngle_NORM",
    "YAML.outletFrontVertexAngle_NORM",
    "YAML.centerCircles[0].backVertexAngle_NORM",
    "YAML.centerCircles[0].frontVertexAngle_NORM",
    "YAML.centerCircles[1].backVertexAngle_NORM",
    "YAML.centerCircles[1].frontVertexAngle_NORM",
    "YAML.centerCircles[2].backVertexAngle_NORM",
    "YAML.centerCircles[2].frontVertexAngle_NORM",
]


# %%
# Your existing data prep:
X = bfDfNorm[input_cols].values.astype("float32")  # input matrix
y = bfDfNorm[output_cols].values.astype("float32")  # output matrix

X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.2, random_state=42
)

input_dim = X_train.shape[1]
output_dim = y_train.shape[1]


# %%
# Define a model-building function for Keras Tuner
def build_model(hp):
    model = tf.keras.Sequential()
    model.add(layers.InputLayer(shape=(input_dim,)))

    # Number of layers fixed to 5 for example
    for i in range(5):
        units = hp.Int(f"units_{i}", min_value=32, max_value=516, step=32)
        model.add(layers.Dense(units, activation="relu"))

    model.add(layers.Dense(output_dim))  # Output layer, linear for regression

    model.compile(optimizer="adam", loss="mse", metrics=["mae"])
    return model


# %%
# Setup tuner
tuner = kt.RandomSearch(
    build_model,
    objective="val_loss",
    max_trials=3,  # Number of models to try
    executions_per_trial=1,
    directory="ktuner_dir",
    project_name="bfnn_3",
)

# %%
# Run tuner search
tuner.search(
    X_train,
    y_train,
    epochs=100,
    validation_data=(X_test, y_test),
    callbacks=[
        callbacks.EarlyStopping(monitor="val_loss", patience=3),
        PlotLossesKeras(),
    ],
    verbose=2,
)

# %%
# Get the best model and summary
best_model = tuner.get_best_models(num_models=1)[0]
best_model.summary()

# %%
# Train best model more if you want
MAX_EPOCHS = 1200


def compile_and_fit(model, X_train, y_train, X_val, y_val, patience=9, earlystop=False):
    early_stopping = callbacks.EarlyStopping(
        monitor="val_loss", patience=patience, mode="min"
    )

    model.compile(
        loss=tf.keras.losses.MeanSquaredError(),
        optimizer=tf.keras.optimizers.Adam(),
        metrics=[
            tf.keras.metrics.MeanAbsoluteError(),
            tf.keras.metrics.MeanAbsolutePercentageError(),
        ],
    )

    cbs = [callbacks.TerminateOnNaN(), PlotLossesKeras()]
    if earlystop:
        cbs.append(early_stopping)

    history = model.fit(
        X_train,
        y_train,
        epochs=MAX_EPOCHS,
        validation_data=(X_val, y_val),
        callbacks=cbs,
        verbose=2,
    )

    return history


# %%
# Optional: further training with the best model
history = compile_and_fit(
    best_model, X_train, y_train, X_test, y_test, patience=10, earlystop=True
)

# %%
# Save the best model
now = datetime.now().strftime("%Y%m%d_%H%M%S")
best_model.save(f"best_model_{now}.keras")

print(f"Model saved as best_model_{now}.keras")
