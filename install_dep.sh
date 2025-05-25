# !/bin/zsh

sudo pacman -S vulkan-headers glfw glm

yay -S imgui-full

mkdir xtllike
cd xtllike

git clone --branch 0.8.0 https://github.com/xtensor-stack/xtl.git
cd xtl
cmake -Bbuild -DCMAKE_INSTALL_PREFIX=$HOME/.local
cmake --build build
cmake --install build
cd ..

git clone --branch 0.26.0 https://github.com/xtensor-stack/xtensor.git
cd xtensor
cmake -Bbuild -DCMAKE_INSTALL_PREFIX=$HOME/.local
cmake --build build
cmake --install build
cd ..


git clone --branch 0.22.0 https://github.com/xtensor-stack/xtensor-blas.git
cd xtensor-blas
cmake -Bbuild -DCMAKE_INSTALL_PREFIX=$HOME/.local
cmake --build build
cmake --install build
cd ..

ls ~/.local/include/xtl
ls ~/.local/include/xtensor
ls ~/.local/include/xtensor-blas





