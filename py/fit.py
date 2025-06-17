import os
from pprint import pprint

import pandas as pd
import yaml


class GridProfile:
    """
    Класс, храняющий основные параметры турбинных сопловых и рабочих решеток МЭИ.
    """

    __data = pd.read_excel(GridsFilePath)

    def __init__(
        self,
        M: float = 0.5,
        type=0,
        in_angle: float = 90,
        out_angle: float = 14,
        name: str = "",
    ):
        try:
            # Копируем необходимую строку параметров для выбранной решетки
            if name != "":
                loc_par = self.__data[self.__data["Name"] == name]
                self.__name = name
            else:
                # Ограничение на включенные решетки (ручное отключение в таблице)
                loc_par = self.__data[self.__data["isOn"] == 1]

                # Выбор типа решетки: сопловая/рабочая
                loc_par = loc_par[loc_par["type"] == type]

                # Ограничение по числу Маха
                loc_par = loc_par[(loc_par["M_b"] <= M) & (loc_par["M_e"] > M)]

                # Ограничение по входному углу
                loc_par = loc_par[
                    (loc_par["out_angle_b"] <= out_angle)
                    & (loc_par["out_angle_e"] > out_angle)
                ]

                # Ограничение по выходному углу
                loc_par = loc_par[
                    (loc_par["in_angle_b"] <= in_angle)
                    & (loc_par["in_angle_e"] > in_angle)
                ]

            # self.__grids_list = loc_par

            # Обновляем индексы строк
            # self.__grids_list.reset_index(drop=True, inplace=True)
            loc_par.reset_index(drop=True, inplace=True)

            if loc_par.empty:
                self.__name_list = []
            else:
                self.__name_list = loc_par["Name"].tolist()

            # Если строка найдена не одна или не найдена
            # if (len(loc_par) != 1):
            #     raise Exception("Invalid number of founded grids, number of grids: {}".format(len(loc_par)))

            self.__name = loc_par.loc[0, "Name"]
            self.__existing_grid = ExistingGrid(
                b=float(loc_par.loc[0, "b"]),
                B=float(loc_par.loc[0, "B"]),
                min_sec_length=float(loc_par.loc[0, "a"]),
                rel_t=float(loc_par.loc[0, "rel_t"]),
                ins_angle=float(loc_par.loc[0, "ins_angle"]),
                in_edge=float(loc_par.loc[0, "Delta_in"]),
                out_edge=float(loc_par.loc[0, "Delta_out"]),
                x_weight_center=float(loc_par.loc[0, "center_X"]),
                y_weight_center=float(loc_par.loc[0, "center_Y"]),
                profile_area=float(loc_par.loc[0, "F"]),
                inertia_moment_xx=float(loc_par.loc[0, "Ixx"]),
                inertia_moment_yy=float(loc_par.loc[0, "Iyy"]),
                resistance_moment_xx_back=float(loc_par.loc[0, "Wxx_back"]),
                resistance_moment_xx_edge=float(loc_par.loc[0, "Wxx_edge"]),
                resistance_moment_yy_in_edge=float(loc_par.loc[0, "Wyy_in_edge"]),
                resistance_moment_yy_out_edge=float(loc_par.loc[0, "Wyy_out_edge"]),
                inst_angle_expr=str(loc_par.loc[0, "ins_angle_expression"]),
            )

        except Exception as ex:
            pass
            # print(*ex.args)

    def __str__(self) -> str:
        loc_str = ""
        # loc_str += 'name = {}\n'.format(str(self.__name.to_list()[0]))
        loc_str += str(self.__name) + "\n"
        loc_str += str(self.__existing_grid)
        return loc_str

    # Getters
    def get_name(self):
        return self.__name

    def get_name_list(self):
        return self.__name_list

    def get_grids_list(self):
        return self.__grids_list

    def get_b(self):
        return self.__existing_grid.get_b()

    def get_B(self):
        return self.__existing_grid.get_B()

    def get_a(self):
        return self.__existing_grid.get_a()

    def get_rel_t(self):
        return self.__existing_grid.get_rel_t()

    def get_standart_ins_angle(self):
        return self.__existing_grid.get_standart_ins_angle()

    def get_Delta_in(self):
        return self.__existing_grid.get_Delta_in()

    def get_Delta_out(self):
        return self.__existing_grid.get_Delta_out()

    def get_center_X(self):
        return self.__existing_grid.get_center_X()

    def get_center_Y(self):
        return self.__existing_grid.get_center_Y()

    def get_F(self):
        return self.__existing_grid.get_F()

    def get_Ixx(self):
        return self.__existing_grid.get_Ixx()

    def get_Iyy(self):
        return self.__existing_grid.get_Iyy()

    def get_Wxx_back(self):
        return self.__existing_grid.get_Wxx_back()

    def get_Wxx_edge(self):
        return self.__existing_grid.get_Wxx_edge()

    def get_Wyy_in_edge(self):
        return self.__existing_grid.get_Wyy_in_edge()

    def get_Wyy_out_edge(self):
        return self.__existing_grid.get_Wyy_out_edge()

    def calculate_inst_angle(self, in_angle, t_rel, isAcc=True):
        if isAcc:
            if "angle" in self.__existing_grid.get_inst_angle_expr():
                return eval(
                    self.__existing_grid.get_inst_angle_expr().format(
                        angle=in_angle, t_rel=t_rel
                    )
                )
            else:
                return float(self.__existing_grid.get_inst_angle_expr())
        else:
            return self.get_standart_ins_angle()

    def isOK(self):
        if len(self.__name_list) != 0:
            return True
        else:
            return False


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


if __name__ == "__main__":
    # files = filesList("/home/snj/Data/Sections/")
    # pprint(readSingleFile(files[0]))
    print(GridsFilePath)
