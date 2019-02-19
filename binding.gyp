{
  "targets": [{
    "cflags": [
      "-std=c99"
    ],
    "target_name": "h3node",
    "include_dirs": [
      "h3/src/h3lib/include"
    ],
    "sources": [
      "h3napi.c",
    ],
    "conditions": [
      [
        "OS==\"linux\"",
        {
          "libraries": [
            "../h3/lib/libh3.a"
          ]
        }
      ],
      [
        "OS==\"mac\"",
        {
          "libraries": [
            "../h3/lib/libh3.a"
          ]
        }
      ],
      [
        "OS==\"win\"",
        {
          "libraries": [
            "msvcrt.lib",
            "../h3/bin/Release/h3.lib"
          ]
        }
      ]
    ]
  }]
}