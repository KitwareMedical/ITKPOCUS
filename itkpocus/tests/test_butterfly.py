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
        #  with self.subTest(i=i):
        #      self.assertEqual()...
        img, meta_dict = itkpocus.butterfly.load_and_preprocess_image('./tests/data/butterfly_axial-lateral-resolution-2020.png')
        
        spacing = meta_dict['spacing']
        crop = meta_dict['crop']
        
        self.assertAlmostEqual(spacing[0], 0.047619, delta=SPACING_DELTA)
        self.assertAlmostEqual(spacing[1], 0.047619, delta=SPACING_DELTA)
    
        self.assertAlmostEqual(crop[0,0], 31)
        self.assertAlmostEqual(crop[0,1], 1048)
        print(crop)
    
    def test_vread_workaround(self):
        f = './tests/data/butterfly-vsync.mp4'
        x = itkpocus.butterfly.vread_workaround(f)
        self.assertAlmostEqual(x.shape[0], 761)
        self.assertAlmostEqual(x.shape[1], 1080)
        self.assertAlmostEqual(x.shape[2], 776)
        self.assertAlmostEqual(x.shape[3], 3)
    
if __name__ == '__main__':
    unittest.main()
