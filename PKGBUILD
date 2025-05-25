pkgname=bladeforge
pkgver=0.0.1
pkgrel=1
arch=('x86_64')
url="https://github.com/Sh1nJiTEITA/BladeForge"
license=('GPL3')
depends=('boost' 'glm' 'vulkan-headers' 'glfw')
makedepends=('cmake' 'gcc' 'git')
source=('git+https://github.com/Sh1nJiTEITA/BladeForge.git::BladeForge')
md5sums=('SKIP')

prepare() {
  cd "$srcdir/BladeForge"
  git submodule update --init --recursive

# Build xtl
  git clone --branch 0.8.0 https://github.com/xtensor-stack/xtl.git "$srcdir/xtl"
  cd "$srcdir/xtl"
  cmake -Bbuild -DCMAKE_INSTALL_PREFIX="$srcdir/local"
  cmake --build build
  cmake --install build
  cd "$srcdir"

  # Build xtensor
  git clone --branch 0.26.0 https://github.com/xtensor-stack/xtensor.git "$srcdir/xtensor"
  cd "$srcdir/xtensor"
  cmake -Bbuild -DCMAKE_INSTALL_PREFIX="$srcdir/local"
  cmake --build build
  cmake --install build
  cd "$srcdir"

  # Build xtensor-blas
  git clone --branch 0.22.0 https://github.com/xtensor-stack/xtensor-blas.git "$srcdir/xtensor-blas"
  cd "$srcdir/xtensor-blas"
  cmake -Bbuild -DCMAKE_INSTALL_PREFIX="$srcdir/local"
  cmake --build build
  cmake --install build
  cd "$srcdir"
}

build() { 
  cd "$srcdir/BladeForge"
  cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$srcdir/local
  cmake --build build
}

package() {
  cd "$srcdir/BladeForge"
  install -Dm755 build/BladeForge "$pkgdir/usr/bin/BladeForge"
}


