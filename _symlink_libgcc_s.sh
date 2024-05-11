# make symlink needed building static linked executable

echo "makeing symlink of /usr/lib/libgcc_s.so --> /usr/lib/libgcc_s.so.1"

sd ln -sf /usr/lib/x86_64-linux-gnu/libgcc_s.so.1 /usr/lib/x86_64-linux-gnu/libgcc_s.so

ls -l /usr/lib/x86_64-linux-gnu/libgcc_s.*
