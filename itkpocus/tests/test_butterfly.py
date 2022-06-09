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
    
        self.assertAlmostEqual(crop[0,0], 36)
        self.assertAlmostEqual(crop[0,1], 1062)
        print(crop)


    def test_mean_crop(self):
        img, _ = itkpocus.butterfly.load_and_preprocess_video('./tests/data/butterfly-mean-crop.mp4')
        img_size = img.GetLargestPossibleRegion().GetSize()
        self.assertNotEqual(img_size[0], 0)
        self.assertNotEqual(img_size[1], 0)

    
    def test_vread_workaround(self):
        f = './tests/data/butterfly-vsync.mp4'
        x = itkpocus.butterfly.vread_workaround(f)
        self.assertAlmostEqual(x.shape[0], 761)
        self.assertAlmostEqual(x.shape[1], 1080)
        self.assertAlmostEqual(x.shape[2], 776)
        self.assertAlmostEqual(x.shape[3], 3)
    

if __name__ == '__main__':
    unittest.main()
