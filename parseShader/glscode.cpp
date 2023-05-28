#include <stdio.h>
#include <string.h>
#include <iostream>

#include "glscode.hpp"


ShaderCode::ShaderCode(const char* filepath, const char* ind) {
    size_t len = 0, clen = 0;
    FILE* pshc = fopen(filepath, "r");
    char* line = nullptr, *target = nullptr, *cstr = nullptr;

    if (pshc == nullptr) perror("Unable to open file");

    while (getline(&line, &len, pshc) != -1) {
        if (memcmp(line, ind, sizeof(ind))) {
            size_t llen = strlen(line);
            cstr = (char*) realloc(cstr, cstr == nullptr? llen+1: clen+llen+1);
            memcpy(cstr+clen, line, llen+1);
            clen += llen;
        }
        else {
            if (cstr != nullptr) {
                this->shadermap.insert(std::make_pair(target, cstr));
                cstr = nullptr;
            }
            size_t tlen = strlen(line + strlen(ind));
            target = (char*) malloc(tlen);
            memcpy(target, line + strlen(ind), tlen);
            target[tlen-1] = '\0', clen = 0;
        }
    }
    if (cstr != nullptr) this->shadermap.insert(std::make_pair(target, cstr));
    free(line);
    fclose(pshc);
}

ShaderCode::~ShaderCode() {
    auto psm = this->shadermap.begin();
    while (psm!=this->shadermap.end()) free((char*)psm->second), psm++;
    this->shadermap.clear();
}
