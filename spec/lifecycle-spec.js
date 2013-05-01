var buster = require( "buster" ),
	xmlgoo = require( ".." )
	;

buster.spec.expose();


describe( "Given parsed xml", function() {

	before( function() {

		this.timeout = 2000;
		this.doc = xmlgoo.parse("<root>hello</root>");
		this.documentElement = this.doc.documentElement();

	} );

	describe( "When the document is dereferenced", function() {

		before( function() {

			this.doc = null;
			gc(); gc(); gc();

		} );

		it( "It should still be possible to call xml() on the document element", function() {

			expect( this.documentElement.xml() ).toBeTruthy();

		} );

	} );

	describe( "When the documentElement is dereferenced", function() {

		before( function() {

			this.documentElement = null;
			gc(); gc(); gc();

		} );

		it( "It should still be possible to call xml() on the document", function() {

			expect( this.doc.xml() ).toBeTruthy();

		} );

		describe( "And the documentElement is retrieved again", function() {

			before( function() {

				this.de2 = this.doc.documentElement();
				gc(); gc(); gc();

			} );

			it( "It should still be possible to call xml() on it", function() {

				expect( this.de2.xml() ).toBeTruthy();

			} );

		} );

	} );

	describe( "When a factory function is invoked on another object", function() {

		before( function() {

			var randomScope = { "name" : "random" };
			try {
				this.doc.documentElement.call(randomScope);
			} catch(e) {
				this.caughtException = e;
			}

		} );

		it( "It should still return the correct object", function() {

			expect( this.caughtException ).toBeTruthy();

		} );

	} );

} );

