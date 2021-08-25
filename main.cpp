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
#include <sstream>
#include <ctime>
#include <tclap/CmdLine.h>
#define MEM6502_SIZE 1024*64 + 1

std::unordered_map<std::string, char> opcodes = {
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

std::string determineAddressingMode( const std::string& param ) {
    //Implied - No Suffix
    if ( param == "None" )
        return {};
    //Accumulator - "A" Suffix
    if ( param == "A" || param == "a" )
        return "A";
    //Immediate - "IM" Suffix
    if ( param.find( '#' ) != std::string::npos )
        return "IM";
    if ( param.find('*') != std::string::npos )
        return "R";

    //Everything past this point uses addresses, so lets extract the address from the param.
    std::string address = std::regex_replace( param, std::regex( "[^0-9A-F]*([0-9A-F]+).*" ), std::string( "$1" ));

    //Zero Page - "ZP[X/Y]" Suffix
    if ( address.length() == 2 ) {
        if ( param.find( 'X' ) != std::string::npos || param.find( 'x' ) != std::string::npos )
            return "ZPX";
        if ( param.find( 'Y' ) != std::string::npos || param.find( 'y' ) != std::string::npos )
            return "ZPY";
        return "ZP";
    }
    //Absolute - "AB[X/Y]" Suffix
    if ( address.length() == 4 && param.find( '(' ) == std::string::npos ) {
        if ( param.find( 'X' ) != std::string::npos || param.find( 'x' ) != std::string::npos )
            return "ABX";
        if ( param.find( 'Y' ) != std::string::npos || param.find( 'y' ) != std::string::npos )
            return "ABY";
        return "AB";
    }
    //Indirect - "IN[X/Y]" Suffix
    if ( param.find( '(' ) != std::string::npos ) {
        if ( param.find( 'X' ) != std::string::npos || param.find( 'x' ) != std::string::npos )
            return "INX";
        if ( param.find( 'Y' ) != std::string::npos || param.find( 'y' ) != std::string::npos )
            return "INY";
        return "IN";
    }

    return "Error: Unknown Addressing Mode";

}

std::string getNormalizedParam(std::string param ) {
    if ( param == "None" )
        return param;
    std::string address = std::regex_replace( param, std::regex( "[^0-9A-F]*([0-9A-F]+).*" ), std::string( "$1" ));
    if ( param.find( '$' ) != std::string::npos ) {
        return std::to_string( strtol( address.c_str(), nullptr, 16 ) );
    }
    else
        return std::to_string( strtol( address.c_str(), nullptr, 10 ) );
}

void assemble( std::string instruction, std::string param, std::ofstream &outfile, int linecount ) {
    if ( opcodes[instruction] == 0x00 && instruction.find("BRK") == std::string::npos && param != "None" ) {
        std::cout << "Error: Invalid Instruction/Operand at line: " + std::to_string(linecount) + "\n";
        exit(3);
    }
    char outcode = opcodes[instruction];
    outfile.write( &outcode, sizeof(outcode) );
    if ( param == "None" )
        return;
    std::string normalized_param = getNormalizedParam(param);
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
    try {
        time_t timestart = time( nullptr );
        bool debug;
        std::string input;
        std::string output;
        TCLAP::CmdLine cmdLine( "6502as", ' ', "0.2a" );

        TCLAP::SwitchArg debugSwitch("v", "verbose", "Prints information while assembling file", cmdLine, false);
        TCLAP::ValueArg<std::string> outputPath("o", "output", "Specifies output filename, default is \"a.out6502\"", false, "a.out6502", "filename");
        TCLAP::UnlabeledValueArg<std::string> inputPath("input", "Input file filename", true, "", "filename");
        cmdLine.add( outputPath );
        cmdLine.add( inputPath );
        cmdLine.parse( argc, argv );

        debug = debugSwitch.getValue();
        input = inputPath.getValue();
        output = outputPath.getValue();

        //Set-up vars:

        //File: the input file name
        std::ifstream File;
        File.open( input );
        if(File.fail()){
            printf("Error: File %s does not exist\nExiting", argv[argc-1]);
            exit(3);
        }
        //OutFile: the output file name
        std::ofstream OutFile;
        char buf[MEM6502_SIZE];
        OutFile.open( output, std::ios::binary | std::ios::out );
        OutFile.rdbuf()->pubsetbuf( buf, sizeof buf );
        //line: the current line buffer
        std::string line;
        //linecount: the number of lines since the beginning of the file (for error reporting)
        int linecount = 1;
        //pos: the current position in the current line
        int pos;
        //instruction: std::string that will hold the instruction after being extracted from the line std::string
        std::string instruction;
        //param: std::string that will hold the parameter after being extracted from the line std::string
        std::string param;

        while ( getline( File, line ) ){

            //Make sure there is no whitespace
            if ( all_of( line.begin(), line.end(), ::isspace ) ){
                if ( debug )
                    std::cout << "skip whitespace only at line: " + std::to_string( linecount ) + "\n\n";
                linecount++;
                continue;
            }

            //We are ignoring labels for now
            //TODO: add label support
            if ( line.back() == ':' ){
                if ( debug )
                    std::cout << "skipping label at line: " + std::to_string( linecount ) + "\n\n";
                linecount++;
                continue;
            }

            //Skip comments (starts with ;)
            if ( line.front() == ';' ){
                linecount++;
                continue;
            }

            //Set-up variables for getting ins and param from the std::string
            pos = 0;
            instruction = "";
            param = "";

            //Get the instruction and params from the std::string using the format: [3 letter ins] [whitespace] [param]
            //instruction
            while ( pos <= 3 ){
                instruction += line[pos];
                pos++;
            }

            //skip whitespace
            while ( isspace( line[pos] ) && pos < line.length() ){
                pos++;
            }

            //param
            while ( !isspace( line[pos] ) && pos < line.length() ){
                param += line[pos];
                pos++;
            }

            if ( param.empty() )
                param = "None";

            //6502 has 13 different addressing modes which must be known for the correct opcode to be selected
            std::string addr_mode = determineAddressingMode( param );
            if ( addr_mode.find( "Error" ) != std::string::npos ){
                std::cout << addr_mode + "\n";
                std::cout << "Found at line: " + std::to_string( linecount ) + "\n";
                exit(2);
            }

            //All parameters must be normalized for the final assembly step
            std::string normalized_param = getNormalizedParam(param);
            std::string fullIns;
            fullIns += instruction;
            fullIns += addr_mode;
            fullIns.erase(remove_if(fullIns.begin(), fullIns.end(), ::isspace), fullIns.end());
            std::stringstream hexParamStream;
            hexParamStream << std::hex << strtol( normalized_param.c_str(), nullptr, 10 );


            //Informational Output
            if ( debug ) {
                std::cout << "Instruction      : " + instruction + "\n";
                std::cout << "Parameter        : " + param + "\n";
                std::cout << "Hex Param        : " + hexParamStream.str() + "\n";
                std::cout << "Addressing Mode  : " + addr_mode + "\n";
                std::cout << "Full Instruction : " + fullIns + "\n\n";
            }

            //Actually turn the mnemonic and param into machine code (hopefully with no mistakes >_<)
            assemble( fullIns, param, OutFile, linecount );

            //We want to increment linecount after we assemble so that in case something goes wrong,
            //we will report the correct line number.
            linecount++;
        }


        std::cout << "Done, nya~\n";
        time_t timetotal = time(nullptr) - timestart;
        std::cout << "Time End: " + std::to_string(timetotal) + "\n";
        return 0;
    }
    catch( TCLAP::ArgException &e ){
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }
}