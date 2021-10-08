import unittest
import itkpocus.clarius

SPACING_DELTA = 0.0005


class TestClarius(unittest.TestCase):
    # def setUp(self):
#         pass
    
#     def teardDown(self):
#         pass
    def test_load_and_preprocess_image(self):
        # consider using subTests in the future for different versions/images
        #  with self.subTest(i=i):
        #      self.assertEqual()...
        
        img, meta_dict = itkpocus.clarius.load_and_preprocess_image('./tests/data/clarius-ocular-image.png')
        spacing = meta_dict['spacing']
        crop = meta_dict['crop']
        self.assertAlmostEqual(spacing[0], 0.2564, delta=SPACING_DELTA)
        self.assertAlmostEqual(spacing[1], 0.2564, delta=SPACING_DELTA)
    
        self.assertAlmostEqual(crop[0,0], 87)
        self.assertAlmostEqual(crop[0,1], 872)
        print(crop)
        
    def test_load_and_preprocess_video(self):
    # consider using subTests in the future for different versions/images
    #  with self.subTest(i=i):
    #      self.assertEqual()...

        img, meta_dict = itkpocus.clarius.load_and_preprocess_video('tests/data/clarius-ocular-short.mp4')
        spacing = meta_dict['spacing']
        crop = meta_dict['crop']
        self.assertAlmostEqual(spacing[0], 0.2564, delta=SPACING_DELTA)
        self.assertAlmostEqual(spacing[1], 0.2564, delta=SPACING_DELTA)
        self.assertAlmostEqual(spacing[2], 0.04, delta=SPACING_DELTA)
    
        self.assertAlmostEqual(crop[0,0], 79)
        self.assertAlmostEqual(crop[0,1], 881)
        print(crop)
    
if __name__ == '__main__':
    unittest.main()
    
    