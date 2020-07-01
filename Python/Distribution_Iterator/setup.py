import platform
import setuptools
from setuptools.dist import Distribution

with open("README.md", "r") as fh:
    long_description = fh.read()

data_files = []

if platform.system() == "Windows":
    data_files.append("mdf_iter.pyd")
else:
    data_files.append("mdf_iter.so")


# NOTE: Seems to be several ways to force wheel to create a ABI specific package.
# The solution from https://stackoverflow.com/a/62667991 is used, as this is closest to the approach required here.
# The solution from https://stackoverflow.com/a/45150383 is another possible path.
class BinaryDistribution(Distribution):
    """Distribution which always forces a binary package with platform name"""
    def has_ext_modules(foo):
        return True


setuptools.setup(
    name="mdf_iter",
    version="0.0.1",
    author="Magnus Aagaard Sørensen",
    author_email="mas@csselectronics.com",
    distclass=BinaryDistribution,
    description="Iterator for MDF files containing bus logged data in CAN or LIN",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/CSS-Electronics/mdf4-converters",
    packages=setuptools.find_packages(),
    classifiers=[
        "License :: OSI Approved :: MIT License",
        "Development Status :: 3 - Alpha",
        "Environment :: Console",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX :: Linux",
        "Programming Language :: C++"
        "Programming Language :: Python :: 3.4",
        "Programming Language :: Python :: 3.5",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
    ],
    python_requires='>=3.4',
    package_data={
        "mdf_iter": data_files
    },
)
