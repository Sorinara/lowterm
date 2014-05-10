clear
\rm page_map.o page_map 2>/tmp/null
gcc -Wall  -g -o page_map page_map.c && ./page_map
#gcc -c page_map.c
#gcc -Wall -g -o key_test key_test.c page_map.o `pkg-config --cflags --libs gtk+-2.0 gthread-2.0` -lX11 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include && ./key_test
