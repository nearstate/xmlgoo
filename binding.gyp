{

	"targets": [{

		"target_name": "xmlgoo",

		"sources": [

			"src/GooDoc.cc",
			"src/GooDoc.h",
			"src/Goo.cc",
			"src/Goo.h",
			"src/GooNode.cc",
			"src/GooNode.h",
			"src/main.cc",

		],

		"libraries": [

			"<!@(xml2-config --libs)"

		],

		"include_dirs": [

			"include"

		],

		"cflags!": [

			"-fno-exceptions"

		],

      	"cflags_cc!": [

      		"-fno-exceptions"

      	],

      	"conditions": [[

        	'OS=="mac"', {

          		"xcode_settings": {

            		"GCC_ENABLE_CPP_EXCEPTIONS": "YES"
          		}
        	}

        ]]

	}]

}