{
  "targets": [{
    "target_name": "h3-node",
    "include_dirs": [
      "h3/src/h3lib/include"
    ],
    "sources": [
      "h3-napi.c",
    ],
    "libraries": [
      "../h3/lib/libh3.a"
    ]
  }]
}