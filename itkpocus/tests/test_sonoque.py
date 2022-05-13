import unittest
import itkpocus.sonoque

SPACING_DELTA = 0.0005


class TestSonoque(unittest.TestCase):
    # def setUp(self):
#         pass
    
#     def teardDown(self):
#         pass
    def test_load_and_preprocess_image(self):
        # consider using subTests in the future for different versions/images
        #  with self.subTest(i=i):
        #      self.assertEqual()...
        img, meta_dict = itkpocus.sonoque.load_and_preprocess_image('./tests/data/sonoque_anechoic-3mm-3mm.dcm')
        
        spacing = meta_dict['spacing']
        crop = meta_dict['crop']
        
        self.assertAlmostEqual(spacing[0], 0.00291, delta=SPACING_DELTA)
        self.assertAlmostEqual(spacing[1], 0.00291, delta=SPACING_DELTA)
    
        self.assertAlmostEqual(crop[0,0], 72)
        self.assertAlmostEqual(crop[0,1], 1382)
        print(crop)

    def test_mean_crop(self):
        img, _ = itkpocus.sonoque.load_and_preprocess_video('./tests/data/sonoque-mean-crop.MOV')
        img_size = img.GetLargestPossibleRegion().GetSize()
        self.assertGreaterEqual(img_size[0], 900)
        self.assertGreaterEqual(img_size[1], 1200)
    
if __name__ == '__main__':
    unittest.main()
