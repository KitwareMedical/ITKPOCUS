import unittest
import itkpocus.sonivate

SPACING_DELTA = 0.0005


class TestSonivate(unittest.TestCase):
    # def setUp(self):
#         pass
    
#     def teardDown(self):
#         pass
    def test_load_and_preprocess_image(self):
        # consider using subTests in the future for different versions/images
        #  with self.subTest(i=i):
        #      self.assertEqual()...
        img, meta_dict = itkpocus.sonivate.load_and_preprocess_image('./tests/data/sonivate-anechoic_1-1.nrrd')
        
        spacing = meta_dict['spacing']
        crop = meta_dict['crop']
        
        self.assertAlmostEqual(spacing[0], 0.0776016, delta=SPACING_DELTA)
        self.assertAlmostEqual(spacing[1], 0.0776016, delta=SPACING_DELTA)
    
        self.assertAlmostEqual(crop[0,0], 0)
        self.assertAlmostEqual(crop[0,1], 511)
        self.assertAlmostEqual(crop[1,0], 133)
        self.assertAlmostEqual(crop[1,1], 378)
        print(crop)
        
    def test_load_and_preprocess_video(self):
        # consider using subTests in the future for different versions/images
        #  with self.subTest(i=i):
        #      self.assertEqual()...
        img, meta_dict = itkpocus.sonivate.load_and_preprocess_video('./tests/data/sonivate-snr-1.mha')
        
        spacing = meta_dict['spacing']
        crop = meta_dict['crop']
        
        self.assertAlmostEqual(spacing[0], 0.0776016, delta=SPACING_DELTA)
        self.assertAlmostEqual(spacing[1], 0.0776016, delta=SPACING_DELTA)
        self.assertAlmostEqual(spacing[2], 1/24.0, delta=SPACING_DELTA)
    
        self.assertAlmostEqual(crop[0,0], 0)
        self.assertAlmostEqual(crop[0,1], 511)
        self.assertAlmostEqual(crop[1,0], 133)
        self.assertAlmostEqual(crop[1,1], 378)
        print(crop)
    
if __name__ == '__main__':
    unittest.main()
