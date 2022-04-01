import unittest
import itkpocus.interson

SPACING_DELTA = 0.0005


class TestIntersons(unittest.TestCase):
    # def setUp(self):
#         pass
    
#     def teardDown(self):
#         pass
    def test_load_and_preprocess_image(self):
        # consider using subTests in the future for different versions/images
        #  with self.subTest(i=i):
        #      self.assertEqual()...
        
        img, meta_dict = itkpocus.interson.load_and_preprocess_image('./tests/data/interson-phantom.png')
        spacing = meta_dict['spacing']
        crop = meta_dict['crop']
        self.assertAlmostEqual(spacing[0], 0.0588, delta=SPACING_DELTA)
        self.assertAlmostEqual(spacing[1], 0.0588, delta=SPACING_DELTA)
    
        self.assertAlmostEqual(crop[0,0], 393)
        self.assertAlmostEqual(crop[0,1], 895)
        print(crop)
        
    def test_load_and_preprocess_video(self):
    # consider using subTests in the future for different versions/images
    #  with self.subTest(i=i):
    #      self.assertEqual()...

        img, meta_dict = itkpocus.interson.load_and_preprocess_video('tests/data/interson-video.cine')
        spacing = meta_dict['spacing']
        crop = meta_dict['crop']
        self.assertAlmostEqual(spacing[0], 0.04882, delta=SPACING_DELTA)
        self.assertAlmostEqual(spacing[1], 0.04882, delta=SPACING_DELTA)
        self.assertAlmostEqual(spacing[2], 0.05882, delta=SPACING_DELTA)
    
        self.assertAlmostEqual(crop[0,0], 0)
        self.assertAlmostEqual(crop[0,1], 0)
        self.assertAlmostEqual(crop[1,0], 686)
        self.assertAlmostEqual(crop[1,1], 1024)
        print(crop)
    
if __name__ == '__main__':
    unittest.main()
    
    