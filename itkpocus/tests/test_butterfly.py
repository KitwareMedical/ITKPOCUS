import unittest
import itkpocus.butterfly

SPACING_DELTA = 0.0005


class TestButterfly(unittest.TestCase):
    # def setUp(self):
#         pass
    
#     def teardDown(self):
#         pass
    def test_load_and_preprocess_image(self):
        # consider using subTests in the future for different versions/images
#         with self.subTest(i=i):
#             self.assertEqual()...
        img, spacing, crop = itkpocus.butterfly.load_and_preprocess_image('./tests/data/butterfly_axial-lateral-resolution-2020.png')
        self.assertAlmostEqual(spacing[0], 0.047619, delta=SPACING_DELTA)
        self.assertAlmostEqual(spacing[1], 0.047619, delta=SPACING_DELTA)
    
        self.assertAlmostEqual(crop[0,0], 54)
        self.assertAlmostEqual(crop[0,1], 10)
        print(crop)


    
if __name__ == '__main__':
    unittest.main()
