#!/usr/bin/env python3
#############################################################################
#
# These are maps used in code generation
#############################################################################

'''
Helper methods & common code for the rabit message templates msg.{cpp,h,cs}.em

'''

type_map = {
    'int8': 'int8_t',
    'int16': 'int16_t',
    'int32': 'int32_t',
    'int64': 'int64_t',
    'uint8': 'uint8_t',
    'uint16': 'uint16_t',
    'uint32': 'uint32_t',
    'uint64': 'uint64_t',
    'float32': 'float',
    'float64': 'double',
    'bool': 'bool',
    'char': 'char',
    'string': 'std::string',
}

type_map_cs = {
    'int8': 'Int8',
    'int16': 'Int16',
    'int32': 'Int32',
    'int64': 'Int64',
    'uint8': 'UInt8',
    'uint16': 'UInt16',
    'uint32': 'UInt32',
    'uint64': 'UInt64',
    'float32': 'float',
    'float64': 'double',
    'bool': 'bool',
    'char': 'char',
    'string': 'string',
}

type_map_py = {
    'int8': 'b',
    'int16': 'h',
    'int32': 'i',
    'int64': 'q',
    'uint8': 'B',
    'uint16': 'H',
    'uint32': 'I',
    'uint64': 'Q',
    'float32': 'f',
    'float64': 'd',
    'bool': '?',
    'char': 'c',
    'string': 's',
}

type_byte_writer_map = {
    'int8': 'Int8',
    'int16': 'Int16',
    'int32': 'Int32',
    'int64': 'Int64',
    'uint8': 'UInt8',
    'uint16': 'UInt16',
    'uint32': 'UInt32',
    'uint64': 'UInt64',
    'float32': 'Float',
    'float64': 'Double',
    'bool': 'Bool',
    'char': 'Char',
    'string': 'String'
}

init_cpp = {
    'int8': '0',
    'int16': '0',
    'int32': '0',
    'int64': '0',
    'uint8': '0',
    'uint16': '0',
    'uint32': '0',
    'uint64': '0',
    'float32': '0',
    'float64': '0',
    'bool': 'false',
    'char': '0',
    'string': '\"\"',
}

init_cs = {
    'int8': '0',
    'int16': '0',
    'int32': '0',
    'int64': '0',
    'uint8': '0',
    'uint16': '0',
    'uint32': '0',
    'uint64': '0',
    'float32': '0',
    'float64': '0',
    'bool': 'false',
    'char': '\'\'',
    'string': '\"\"',
}

init_py = {
    'int8': '0',
    'int16': '0',
    'int32': '0',
    'int64': '0',
    'uint8': '0',
    'uint16': '0',
    'uint32': '0',
    'uint64': '0',
    'float32': '0',
    'float64': '0',
    'bool': 'False',
    'char': '\'\'',
    'string': '\"\"',
}

