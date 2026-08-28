# Reads IN_FILE and writes a header defining a raw string literal VAR_NAME
file(READ ${IN_FILE} content)

# Guard against delimiter collisions in raw string literal — cheap safety check
string(FIND "${content}" ")sigmf_schema\"" collision)
if(NOT collision EQUAL -1)
    message(FATAL_ERROR "Delimiter collision embedding ${IN_FILE}")
endif()

file(WRITE ${OUT_FILE} "// Auto-generated from ${IN_FILE}. Do not edit.\n")
file(APPEND ${OUT_FILE} "#pragma once\n\n")
file(APPEND ${OUT_FILE} "namespace sigmf_io {\n\n")
file(APPEND ${OUT_FILE} "inline constexpr char ${VAR_NAME}[] = R\"sigmf_schema(${content})sigmf_schema\";\n\n")
file(APPEND ${OUT_FILE} "} // namespace sigmf_io\n")
