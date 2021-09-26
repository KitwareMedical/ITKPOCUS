import unittest
import itkpocus.sonosite

SPACING_DELTA = 0.0005


class TestSonosite(unittest.TestCase):
    # def setUp(self):
#         pass
    
#     def teardDown(self):
#         pass
    def test_load_and_preprocess_image(self):
        # consider using subTests in the future for different versions/images
        #  with self.subTest(i=i):
        #      self.assertEqual()...
        img, meta_dict = itkpocus.sonosite.load_and_preprocess_image('./tests/data/sonosite-anon-eye1.jpg')
        
        spacing = meta_dict['spacing']
        crop = meta_dict['crop']
        
        self.assertAlmostEqual(spacing[0], 0.07575758, delta=SPACING_DELTA)
        self.assertAlmostEqual(spacing[1], 0.07575758, delta=SPACING_DELTA)
    
        self.assertAlmostEqual(crop[0,0], 50)
        self.assertAlmostEqual(crop[0,1], 620)
        print(crop)
    
if __name__ == '__main__':
    unittest.main()
