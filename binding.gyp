{
  "targets": [{
    "cflags": [
      "-std=C99"
    ],
    "target_name": "h3node",
    "include_dirs": [
      "h3/src/h3lib/include"
    ],
    "sources": [
      "h3napi.c",
    ],
    "libraries": [
      "../h3/lib/libh3.a"
    ]
  }]
}