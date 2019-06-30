import os

schemas_dir = "./../schemas" #fbs files path
output_dir = "./../../../ShaderBrowser/src/Plugins/flatbuffer/generated_header"	#out path

fileList = "";
for root, dirs, files in os.walk(schemas_dir):
	for file in files:
		# print(file)
		fileList = fileList + schemas_dir + "/" + file + " "
# print fileList

str_cmd = "./flatc -c -o " + output_dir + " " + fileList
print("cmd = " + str_cmd)
os.system(str_cmd)
