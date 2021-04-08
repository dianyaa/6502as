// 6502as, a pretty garbage assembler for 6502
// Created by: Dianya~~
// Created on: 4/8/2021
// github.com/dianyaa/6502as
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <regex>
#include <unordered_map>

using namespace std;

std::unordered_map<string, char> opcodes = {
        {"ADCIM", 0x69}, {"ADCZP", 0x65}, {"ADCZPX", 0x75}, {"ADCAB", 0x6D},
        {"ADCABX", 0x7D}, {"ADCABY", 0x79}, {"ADCINX", 0x61}, {"ADCINY", 0x71},
        {"ANDIM", 0x29}, {"ANDZP", 0x25}, {"ANDZPX", 0x35}, {"ANDAB", 0x2D},
        {"ANDABX", 0x3D}, {"ANDABY", 0x39}, {"ANDINX", 0x21}, {"ANDINY", 0x31},
        {"ASLA", 0x0A}, {"ASLZP", 0x06}, {"ASLZPX", 0x16}, {"ASLAB", 0x0E},
        {"ASLABX", 0x1E}, {"BCCR", 0x90}, {"BCSR", 0xB0}, {"BEQR", 0xF0},
        {"BITZP", 0x24}, {"BITAB", 0x2C}, {"BMIR", 0x30}, {"BNER", 0xD0},
        {"BPLR", 0x10}, {"BRK", 0x00}, {"BVCR", 0x50}, {"BVSR", 0x70}, {"CLC", 0x18},
        {"CLD", 0xD8}, {"CLI", 0x58}, {"CLV", 0xB8}, {"CMPIM", 0xC9}, {"CMPZP", 0xC5},
        {"CMPZPX", 0xD5}, {"CMPAB", 0xCD}, {"CMPABX", 0xDD}, {"CMPABY", 0xD9},
        {"CMPINX", 0xC1}, {"CMPINY", 0xD1}, {"CPXIM", 0xE0}, {"CPXZP", 0xE4},
        {"CPXAB", 0xEC}, {"CPYIM", 0xC0}, {"CPYZP", 0xC4}, {"CPYAB", 0xCC},
        {"DECZP", 0xC6}, {"DECZPX", 0xD6}, {"DECAB", 0xCE}, {"DECABX", 0xDE},
        {"DEX", 0xCA}, {"DEY", 0x88}, {"EORIM", 0x49}, {"EORZP", 0x45}, {"EORZPX", 0x55},
        {"EORAB", 0x4D}, {"EORABX", 0x5D}, {"EORABY", 0x59}, {"EORINX", 0x41},
        {"EORINY", 0x51}, {"INCZP", 0xE6}, {"INCZPX", 0xF6}, {"INCAB", 0xEE},
        {"INCABX", 0xFE}, {"INX", 0xE8}, {"INY", 0xC8}, {"JMPAB", 0x4C}, {"JMPIN", 0x6C},
        {"JSRAB", 0x20}, {"LDAIM", 0xA9}, {"LDAZP", 0xA5}, {"LDAZPX", 0xB5},
        {"LDAAB", 0xAD}, {"LDAABX", 0xBD}, {"LDAABY", 0xB9}, {"LDAINX", 0xA1},
        {"LDAINY", 0xB1}, {"LDXIM", 0xA2}, {"LDXZP", 0xA6}, {"LDXZPY", 0xB6},
        {"LDXAB", 0xAE}, {"LDXABY", 0xBE}, {"LDYIM", 0xA0}, {"LDYZP", 0xA4},
        {"LDYZPX", 0xB4}, {"LDYAB", 0xAC}, {"LDYABX", 0xBC}, {"LSRA", 0x4A},
        {"LSRZP", 0x46}, {"LSRZPX", 0x56}, {"LSRAB", 0x4E}, {"LSRABX", 0x5E},
        {"NOP", 0xEA}, {"ORAIM", 0x09}, {"ORAZP", 0x05}, {"ORAZPX", 0x15}, {"ORAAB", 0x0D},
        {"ORAABX", 0x1D}, {"ORAABY", 0x19}, {"ORAINX", 0x01}, {"ORAINY", 0x11},
        {"PHA", 0x48}, {"PHP", 0x08}, {"PLA", 0x68}, {"PLP", 0x28}, {"ROLA", 0x2A},
        {"ROLZP", 0x26}, {"ROLZPX", 0x36}, {"ROLAB", 0x2E}, {"ROLABX", 0x3E},
        {"RORA", 0x6A}, {"RORZP", 0x66}, {"RORZPX", 0x76}, {"RORAB", 0x6E},
        {"RORABX", 0x7E}, {"RTI", 0x40}, {"RTS", 0x60}, {"SBCIM", 0xE9}, {"SBCZP", 0xE5},
        {"SBCZPX", 0xF5}, {"SBCAB", 0xED}, {"SBCABX", 0xFD}, {"SBCABY", 0xF9},
        {"SBCINX", 0xE1}, {"SBCINY", 0xF1}, {"SEC", 0x38}, {"SED", 0xF8}, {"SEI", 0x78},
        {"STAZP", 0x85}, {"STAZPX", 0x95}, {"STAAB", 0x8D}, {"STAABX", 0x9D},
        {"STAABY", 0x99}, {"STAINX", 0x81}, {"STAINY", 0x91}, {"STXZP", 0x86},
        {"STXZPY", 0x96}, {"STXAB", 0x8E}, {"STYZP", 0x84}, {"STYZPX", 0x94},
        {"STYAB", 0x8C}, {"TAX", 0xAA}, {"TAY", 0xA8}, {"TSX", 0xBA}, {"TXA", 0x8A},
        {"TXS", 0x9A}, {"TYA", 0x98}
};

string determineAddressingMode( const string& param ) {
    //Implied - No Suffix
    if ( param == "None" )
        return {};
    //Accumulator - "A" Suffix
    if ( param == "A" || param == "a" )
        return "A";
    //Immediate - "IM" Suffix
    if ( param.find( '#' ) != string::npos )
        return "IM";
    if ( param.find('*') != string::npos )
        return "R";

    //Everything past this point uses addresses, so lets extract the address from the param.
    string address = regex_replace( param, regex( "[^0-9A-F]*([0-9A-F]+).*" ), string( "$1" ));

    //Zero Page - "ZP[X/Y]" Suffix
    if ( address.length() == 2 ) {
        if ( param.find( 'X' ) != string::npos || param.find( 'x' ) != string::npos )
            return "ZPX";
        if ( param.find( 'Y' ) != string::npos || param.find( 'y' ) != string::npos )
            return "ZPY";
        return "ZP";
    }
    //Absolute - "AB[X/Y]" Suffix
    if ( address.length() == 4 && param.find( '(' ) == string::npos ) {
        if ( param.find( 'X' ) != string::npos || param.find( 'x' ) != string::npos )
            return "ABX";
        if ( param.find( 'Y' ) != string::npos || param.find( 'y' ) != string::npos )
            return "ABY";
        return "AB";
    }
    //Indirect - "IN[X/Y]" Suffix
    if ( param.find( '(' ) != string::npos ) {
        if ( param.find( 'X' ) != string::npos || param.find( 'x' ) != string::npos )
            return "INX";
        if ( param.find( 'Y' ) != string::npos || param.find( 'y' ) != string::npos )
            return "INY";
        return "IN";
    }

    return "Error: Unknown Addressing Mode";

}

string getNormalizedParam(string param ) {
    if ( param == "None" )
        return param;
    string address = regex_replace( param, regex( "[^0-9A-F]*([0-9A-F]+).*" ), string( "$1" ));
    if ( param.find( '$' ) != string::npos ) {
        return to_string( strtol( address.c_str(), nullptr, 16 ) );
    }
    else
        return to_string( strtol( address.c_str(), nullptr, 10 ) );
}

void assemble( string instruction, string param, ofstream &outfile, int linecount ) {
    if ( opcodes[instruction] == 0x00 && instruction.find("BRK") == string::npos && param != "None" ) {
        cout << "Error: Invalid Instruction/Operand at line: " + to_string(linecount) + "\n";
        exit(3);
    }
    char outcode = opcodes[instruction];
    outfile.write( &outcode, sizeof(outcode) );
    if ( param == "None" )
        return;
    string normalized_param = getNormalizedParam(param);
    int addr_num = strtol( normalized_param.c_str(), nullptr, 10 );
    if ( normalized_param.length() > 2 ){
        char upper = ( addr_num >> 8 );
        char lower = ( addr_num & 0xFF );
        outfile.write( &lower, sizeof(lower) );
        outfile.write( &upper, sizeof(upper) );
    }
    else {
        outfile.write( reinterpret_cast<const char *>(&addr_num), sizeof( (char)addr_num) );
    }
}

int main(int argc, char* argv[] ){
    if ( argc <= 1 ){
        puts( "Usage: 6502as [OPTIONS] FILE\n" );
        fflush( stdout );
        exit( 1 );
    }
    bool debug = false;
    for ( int i = 0; i < argc; i++ ){
        string s1(argv[i]);
        if ( s1.find("-h") != std::string::npos || s1.find("--help") != std::string::npos ){
            puts( "Usage: 6502as [OPTIONS] FILE\n" );
            puts( "FILE is any valid 6502 assembly file\n" );
            puts( " --help, -h         Displays this message\n" );
            puts( " --verbose, -v      Enables verbose output\n" );
            exit( 0 );
        }
        else if ( s1.find("-v") != std::string::npos || s1.find("--verbose") != std::string::npos )
            debug = true;
    }

    //Set-up vars:

    //File: the input file name
    ifstream File;
    File.open( argv[1] );
    //OutFile: the output file name
    ofstream OutFile;
    OutFile.rdbuf()->pubsetbuf( nullptr,0 );
    OutFile.open( "a.out6502", ios::binary | ios::out );
    //line: the current line buffer
    string line;
    //linecount: the number of lines since the beginning of the file (for error reporting)
    int linecount = 1;
    //pos: the current position in the current line
    int pos;
    //instruction: string that will hold the instruction after being extracted from the line string
    string instruction;
    //param: string that will hold the parameter after being extracted from the line string
    string param;

    while ( getline( File, line ) ){

        //Make sure there is no whitespace
        if ( all_of( line.begin(), line.end(), ::isspace ) ){
            if ( debug )
                cout << "skip whitespace only at line: " + to_string( linecount ) + "\n\n";
            linecount++;
            continue;
        }

        //We are ignoring labels for now
        //TODO: add label support
        if ( line.back() == ':' ){
            if ( debug )
                cout << "skipping label at line: " + to_string( linecount ) + "\n\n";
            linecount++;
            continue;
        }

        //Skip comments (starts with #)
        if ( line.front() == '#' ){
            linecount++;
            continue;
        }

        //Set-up variables for getting ins and param from the string
        pos = 0;
        instruction = "";
        param = "";

        //Get the instruction and params from the string using the format: [3 letter ins] [whitespace] [param]
        while ( pos <= line.length() ){
            //instruction
            while ( pos <= 3 ){
                instruction += line[pos];
                pos++;
            }

            //skip whitespace
            while ( isspace( line[pos] ) && pos <= line.length() ){
                pos++;
            }

            //param
            while ( !isspace( line[pos] ) && pos <= line.length() ){
                param += line[pos];
                pos++;
            }
        }

        if ( param.empty() )
            param = "None";

        //6502 has 13 different addressing modes which must be known for the correct opcode to be selected
        string addr_mode = determineAddressingMode( param );
        if ( addr_mode.find( "Error" ) != string::npos ){
            cout << addr_mode + "\n";
            cout << "Found at line: " + to_string( linecount ) + "\n";
            exit(2);
        }

        //All parameters must be normalized for the final assembly step
        string normalized_param = getNormalizedParam(param);
        string fullIns;
        fullIns += instruction;
        fullIns += addr_mode;
        fullIns.erase(remove_if(fullIns.begin(), fullIns.end(), ::isspace), fullIns.end());
        stringstream hexParamStream;
        hexParamStream << hex << strtol( normalized_param.c_str(), nullptr, 10 );


        //Informational Output
        //TODO: make informational output toggleable
        if ( debug ) {
            cout << "Instruction      : " + instruction + "\n";
            cout << "Parameter        : " + param + "\n";
            cout << "Hex Param        : " + hexParamStream.str() + "\n";
            cout << "Addressing Mode  : " + addr_mode + "\n";
            cout << "Full Instruction : " + fullIns + "\n";
        }

        //Actually turn the mnemonic and param into machine code (hopefully with no mistakes >_<)
        assemble( fullIns, param, OutFile, linecount );

        //We want to increment linecount after we assemble so that in case something goes wrong,
        //we will report the correct line number.
        linecount++;

    }

    cout << "Done, nya~\n";
    return 0;
}