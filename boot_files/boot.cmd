echo "Trying to load primary image"

fdt addr -c

# Load the fdt first
setenv fdt_addr ${fdtcontroladdr}

# Load the image into memory
if load mmc 0:1 0x00080000 ${image}; then
    echo "Loaded primary image into memory"
    sleep 5
    go 0x00080000 ${fdt_addr}

# If it could not be loaded try to load a backup
else
    echo "Failed to load primary image memory"
    if test "${image}" = "ifs-rpi4.bin"; then

        # Loading the backup image
        echo "Trying to load backup image"
        if load mmc 0:1 0x00080000 ifs-rpi4.bin.bak; then
            echo "Loaded backup image into memory"
            sleep 5
            go 0x00080000 ${fdt_addr}

        # Indicate that no image could be loaded
        else
            echo "Failed to load backup image"
        fi

    # Specify that the boot image failed
    else
        echo "Boot failed for boot image ${image}"
    fi
fi