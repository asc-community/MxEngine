from zipfile import ZipFile

def extract_to(zip_path, dist_folder):
	zf = ZipFile(dist_folder, 'r')
	zf.extractall(zip_path)
	zf.close()
	
print('unzipping python...')
extract_to('./x64/Debug',   './external_libraries/python.zip')
extract_to('./x64/Release', './external_libraries/python.zip')

print('unzipping boost_headers...')
extract_to('.', './external_libraries/boost_headers.zip')

print('unzipping assimp...')
extract_to('./external_libraries', './external_libraries/assimp.zip')

print('unzipping compiled static libraries...')
extract_to('./external_libraries', './external_libraries/compiled-libs.zip')

print('unzipping compiled dll libraries...')
extract_to('./x64/Debug',   './external_libraries/compiled-dlls.zip')
extract_to('./x64/Release', './external_libraries/compiled-dlls.zip')

print('unzipping vendors folder...')
extract_to('./src', './external_libraries/Vendors.zip')

input('succeeded unzipping libraries, press Enter to continue...')