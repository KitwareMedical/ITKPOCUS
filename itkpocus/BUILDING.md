# Building and distributing ITKPOCUS
We use _flit_ and _sphinx_ for distribution and documentation.
 
1.  Setting up itkpocus to be live-installed in your _venv_
```
    # activate venv, i.e., source path/to/venv/Scripts/activate
    flit install --wrap-pth
```

## Building and rendering Jupyter examples
TODO

## Building documentation
```
    # assuming venv-3.7.6-itkpocus is the virtual environment
    source venv-3.7.6-itkpocus/Scripts/activate
    
    # brings in its local build of sphinx
    # exclude tests from doc
    sphinx-apidoc -f -o doc . 'tests'
    ./doc/make html
```

## Running testing
TODO