from setuptools import setup,  find_packages
from setuptools import Extension
from setuptools.command.build_ext import build_ext
import sys
import os

class NoopBuildExt(build_ext):
    def run(self):
        pass  # Don't build anything, just install the existing .so

setup(
    name='neurongraph',
    version='0.1.0',
    author='Your Name',
    author_email='your.email@example.com',
    description='Python bindings for NeuronGraph using pybind11',
    packages=find_packages(where='.'),  # Automatically finds 'python_package' and submodules
    package_data={'python_package': ['neurongraph*.so'],
                  'python_package.bin':['pyneuronviewer']},
    include_package_data=True,
    cmdclass={'build_ext': NoopBuildExt},
    zip_safe=False,
)

