# Building and distributing ITKPOCUS
We use _flit_ and _sphinx_ for distribution and documentation.
 
1.  Setting up itkpocus to be live-installed in your _venv_
```
    # activate venv, i.e., source path/to/venv/Scripts/activate
    flit install --wrap-pth
```

## Building and rendering Jupyter examples
1. Create the notebook in [examples](examples)
2. Add a .nblink file to [doc](doc)
3. Add .nblink file to [doc/index.rst](doc/index.rst)
NOTE: steps #1 and #2 are necessary because _examples_ is outside of the _doc_ directory.

## Building documentation
1. Documentation is built using sphinx, sphinx-napoleon, sphinx-apidoc, and the sphinxdoc html_theme.
```
    # assuming venv-3.7.6-itkpocus is the virtual environment
    source venv-3.7.6-itkpocus/Scripts/activate
    
    # brings in its local build of sphinx
    # exclude tests from doc
    sphinx-apidoc -f -o doc . 'tests'
    ./doc/make html
```
2. Note, all documentation will be rebuilt and deployed to readthedocs _automatically_ on merge to master.

## Running testing
1. `python -m unittest` or `python tests/test_myfile.py`

## Publishing to PyPI
1. Update `itkpocus/__init__.py` version number
    1. Use _rc_ for release candidates on major versions
    2. Use subversions for bugfixes
2. `flit publish`