FROM ghcr.io/wiiu-env/devkitppc:20240505

COPY --from=ghcr.io/wiiu-env/libmocha:20231127 /artifacts $DEVKITPRO

WORKDIR /app
CMD make -j$(nproc)
CMD wuhbtool --name=SplatfestDLPP --short-name=aromaSFDL --author=andrea --icon=sfs.png --tv-image=tv.png --drc-image=sillydrc.png SplatfestDL_Plus.rpx SFDL.wuhb
