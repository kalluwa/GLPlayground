import os

def get_all_files():
    filelist = list(os.listdir("./"))
    
    #get vs
    def get_vs(x):
        return x.endswith(".vs")
    vs_filelist = filter(get_vs,filelist)
    
    #get gs
    def get_gs(x):
        return x.endswith(".gs")
    gs_filelist = filter(get_gs,filelist)
    
    #get fs
    def get_fs(x):
        return x.endswith(".fs")
    fs_filelist = filter(get_fs,filelist)
    
    
    #build all shader
    glsl_exe_path = "\"D:/Program Files/Vulkan1.2.154.1/Bin/glslangValidator.exe\""
    for filename in vs_filelist:
        param = "%s -G %s -S vert --auto-map-locations -o %s.spv"%(glsl_exe_path,filename,filename)
        
        os.system(param)
        
        
    for filename in gs_filelist:
        param = "%s -G %s -S geom --auto-map-locations -o %s.spv"%(glsl_exe_path,filename,filename)
        
        os.system(param)
        
    for filename in fs_filelist:
        param = "%s -G %s -S frag --auto-map-locations -o %s.spv"%(glsl_exe_path,filename,filename)  

        os.system(param)        
        
if __name__ == "__main__":
    get_all_files()