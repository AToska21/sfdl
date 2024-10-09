FROM ghcr.io/wiiu-env/devkitppc:20240704

COPY --from=ghcr.io/wiiu-env/libmocha:20240603 /artifacts $DEVKITPRO

WORKDIR /project
