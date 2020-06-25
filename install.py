from zipfile import ZipFile

def extract_to(zip_path, dist_folder):
	zf = ZipFile(dist_folder, 'r')
	zf.extractall(zip_path)
	zf.close()
	
print('unzipping python...')
extract_to('.', './external_libraries/python.zip')

print('unzipping boost_headers...')
extract_to('.', './external_libraries/boost_headers.zip')

print('unzipping assimp...')
extract_to('./external_libraries', './external_libraries/assimp.zip')

print('unzipping compiled libraries...')
extract_to('./external_libraries', './external_libraries/compiled-libs.zip')

print('unzipping vendors folder...')
extract_to('./src', './external_libraries/Vendors.zip')

input('succeeded unzipping libraries, press Enter to continue...')