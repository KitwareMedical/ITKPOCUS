# Style and Developer's Guide

1. Docstrings are in numpy format [https://numpydoc.readthedocs.io/en/latest/format.html](https://numpydoc.readthedocs.io/en/latest/format.html)
2. No human data or other sensitive data may be included in pull requests without explicit permission from the ITKPOCUS authors.
3. Tests are encouraged using `unittest`.  Small tests images may be included in [tests/data](tests/data).
4. Contributions for other device manufacturers should follow the template of existing devices.
    1. Create a manufacture_name.py
    2. Write `preprocess_` methods that return an `itk.Image[itk.F,]` with intensity scaled to 0.0 to 1.0 and physical dimensions set.
    3. Consider adding a `version` parameter to the `preprocess_` methods for compatibility.
