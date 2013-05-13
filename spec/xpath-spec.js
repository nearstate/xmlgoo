var buster = require( "buster" ),
	xmlgoo = require( ".." )
	;

buster.spec.expose();

describe( "Given xml tree to select with xpath", function() {

	before( function() {

		this.testXml = "<root><hello to=\"be\">world</hello><one><two/><three id='blah'/><four/></one><three /></root>";
		this.doc = xmlgoo.parse( this.testXml );

	} );

	describe( "When single node is selected", function() {

		before( function() {

			this.one = this.doc.documentElement().selectSingleNode( "//one" );

		} );

		it( "it should be the right node", function() {

			expect( this.one ).toBe( this.doc.documentElement().childNodes()[1] );

		} );

		it( "it should be possible to test for the node's name", function() {

			expect( this.one.nodeName() ).toEqual( "one" );

		} );

		describe( "and we search for another node in context", function() {

			before( function() {

				this.three1 = this.one.selectSingleNode( ".//three" );

			} );

			it( "it should find the correct node", function() {

				expect( this.three1 ).toBe( this.doc.documentElement().childNodes()[1].childNodes()[1] );

			} );

			it( "it should return a valid node", function() {

				expect( this.three1.nodeName() ).toEqual( "three" );

			} );

		} );

		describe( "and we search for another node using an attribute match", function() {

			before( function() {

				this.three2 = this.one.selectSingleNode( "./*[@id='blah']" );

			} );

			it( "it should find the correct node", function() {

				expect( this.three2 ).toBe( this.doc.documentElement().childNodes()[1].childNodes()[1] );

			} );

			it( "it should return a valid node", function() {

				expect( this.three2.nodeName() ).toEqual( "three" );

			} );

		} );

		describe( "and we search for a non-existent node using an attribute match", function() {

			before( function() {

				this.noid = this.one.selectSingleNode( "./*[@id='focu']" );

			} );

			it( "it should return undefined", function() {

				expect( this.noid ).not.toBeDefined();

			} );

		} );

	} );

	describe( "When a non-existent node is selected", function() {

		before( function() {

			this.fortyTwo = this.doc.documentElement().selectSingleNode( "/forty[2]" );

		} );

		it( "it should be undefined", function() {

			expect( this.fortyTwo ).not.toBeDefined();

		} );

	} );

	describe( "When many nodes are selected", function() {

		before( function() {

			this.all = this.doc.documentElement().selectNodes("//*");

		} );

		it( "it should return the right number of nodes", function() {

			expect( this.all.length ).toEqual( 7 );

		} );

		it( "it should return the same objects as obtained by traversing the tree", function() {

			expect( this.all[1] ).toBe( this.doc.documentElement().childNodes()[0] );

		} );

	} );

	describe( "When selectNodes matches nothing", function() {

		before( function() {

			this.nada = this.doc.documentElement().selectNodes("an honest man");

		} );

		it( "it should return an empty array", function() {

			expect( this.nada.length ).toEqual( 0 );
			expect( Object.prototype.toString.call(this.nada) ).toMatch( "Array" );

		} );

	} );

	describe( "When selectSingleNode matches nothing", function() {

		before( function() {

			this.nada = this.doc.documentElement().selectSingleNode("//anHonestMan");

		} );

		it( "it should return undefined", function() {

			expect( this.nada ).not.toBeDefined();

		} );

	} );

} );