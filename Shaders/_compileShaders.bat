@echo off

SET vsTarget=vs_3_0
SET psTarget=ps_3_0
SET outPath=../../release/data/shaders

fxc /nologo /T %vsTarget% /O3 /E main /Fo %outPath%/shadowMap.vso shadowMap.vs
fxc /nologo /T %psTarget% /O3 /E main /Fo %outPath%/shadowMap.pso shadowMap.ps
fxc /nologo /T %psTarget% /O3 /E main /Fo %outPath%/deferredBlending.pso deferredBlending.ps
fxc /nologo /T %psTarget% /O3 /E main /Fo %outPath%/deferredLighting.pso deferredLighting.ps
fxc /nologo /T %psTarget% /O3 /E main /Fo %outPath%/deferredAmbientLight.pso deferredAmbientLight.ps
fxc /nologo /T %psTarget% /O3 /E main /Fo %outPath%/deferredGBuffer.pso deferredGBuffer.ps
fxc /nologo /T %vsTarget% /O3 /E main /Fo %outPath%/deferredGBuffer.vso deferredGBuffer.vs
fxc /nologo /T %psTarget% /O3 /E main /Fo %outPath%/fxaa.pso fxaa.ps
fxc /nologo /T %vsTarget% /O3 /E main /Fo %outPath%/quad.vso quad.vs
fxc /nologo /T %psTarget% /O3 /E main /Fo %outPath%/quad.pso quad.ps
fxc /nologo /T %vsTarget% /O3 /E main /Fo %outPath%/particle.vso particle.vs
fxc /nologo /T %psTarget% /O3 /E main /Fo %outPath%/particle.pso particle.ps
fxc /nologo /T %vsTarget% /O3 /E main /Fo %outPath%/gui.vso gui.vs
fxc /nologo /T %psTarget% /O3 /E main /Fo %outPath%/gui.pso gui.ps
fxc /nologo /T %psTarget% /O3 /E main /Fo %outPath%/hdrTonemap.pso hdrTonemap.ps
fxc /nologo /T %psTarget% /O3 /E main /Fo %outPath%/hdrScale.pso hdrScale.ps
fxc /nologo /T %psTarget% /O3 /E main /Fo %outPath%/hdrAdaptation.pso hdrAdaptation.ps
fxc /nologo /T %psTarget% /O3 /E main /Fo %outPath%/hdrDownscale.pso hdrDownscale.ps
fxc /nologo /T %vsTarget% /O3 /E main /Fo %outPath%/skybox.vso skybox.vs
fxc /nologo /T %psTarget% /O3 /E main /Fo %outPath%/skybox.pso skybox.ps
fxc /nologo /T %psTarget% /O3 /E main /Fo %outPath%/bloom.pso bloom.ps
fxc /nologo /T %psTarget% /O3 /E main /Fo %outPath%/blur.pso blur.ps
fxc /nologo /T %psTarget% /O3 /E main /Fo %outPath%/gaussianblur.pso gaussianblur.ps
fxc /nologo /T %psTarget% /O3 /E main /Fo %outPath%/ssao.pso ssao.ps
fxc /nologo /T %vsTarget% /O3 /E main /Fo %outPath%/flare.vso flare.vs
fxc /nologo /T %psTarget% /O3 /E main /Fo %outPath%/flare.pso flare.ps

pause