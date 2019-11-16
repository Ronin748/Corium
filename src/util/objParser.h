//
// Created by Roninkoi on 14.2.2016.
//

#ifndef CORIUM_OBJPARSER_H
#define CORIUM_OBJPARSER_H

#include <corium.h>
#include <mesh.h>

Mesh loadObj(std::string path)
{
        std::ifstream file;

        file.open(path);

        Mesh object;

        if (file.is_open()) {
                std::vector<std::string> words = {""};
                char read = ' ';

                const char WHITE_SPACE(32);

                for (int i = 0; !file.eof(); ++i) {
                        read = file.get();

                        if (read != WHITE_SPACE && read != '\n' && read != '\r') {
                                words[words.size() - 1] += read;
                        }
                        else {
                                words.resize(words.size() + 1);
                        }
                }

                int v = 0;
                int f = 0;
                int vt = 0;

                float r = 1.0f;
                float g = 1.0f;
                float b = 1.0f;

                std::vector<float> verts;
                std::vector<float> texes;

                std::vector<int> vi;
                std::vector<int> ti;

                for (int i = 0; i < words.size(); ++i) {

                        if (words[i] == "v") {
                                if (words.size() > i + 3) {
                                        verts.push_back(atof(words[i + 1].c_str()));
                                        verts.push_back(atof(words[i + 2].c_str()));
                                        verts.push_back(atof(words[i + 3].c_str()));
                                }
                        }
                        if (words[i] == "vt") {
                                if (words.size() > i + 1) {
                                        texes.push_back(atof(words[i + 1].c_str()));
                                        texes.push_back(atof(words[i + 2].c_str()));
                                }
                        }
                        if (words[i] == "f") {
                                if (words.size() > i + 2) {
                                        for (int ii = 0; ii < 3; ++ii) {
                                                std::string indexstr = words[i + 1 + ii];
                                                std::string vstring = "";
                                                std::string tstring = "";
                                                bool vb = true;

                                                for (int is = 0; is < indexstr.size() && (indexstr[is] != '/' || vb); ++is) { // throw normal indices away
                                                        if (indexstr[is] != '/') {
                                                                if (vb) {
                                                                        vstring += indexstr[is];
                                                                }
                                                                else {
                                                                        tstring += indexstr[is];
                                                                }
                                                        }
                                                        else {
                                                                vb = false;
                                                        }
                                                }

                                                int verti = atoi(vstring.c_str()) - 1;
                                                int texi = atoi(tstring.c_str()) - 1;

                                                bool exists = false;
                                                int t = 0;
                                                for (; !exists && t < vi.size(); ++t) {
                                                        if (verti == vi[t] && texi == ti[t]) {
                                                                exists = true;
                                                                t -= 1;
                                                        }
                                                }

                                                if (exists) {
                                                        object.indexData.push_back(t);
                                                }
                                                else {
                                                        vi.push_back(verti);
                                                        ti.push_back(texi);
                                                        int ei = vi.size()-1;

                                                        object.vertexData0.push_back(verts[vi[ei] * 3]);
                                                        object.vertexData0.push_back(verts[vi[ei] * 3 + 1]);
                                                        object.vertexData0.push_back(verts[vi[ei] * 3 + 2]);
                                                        object.vertexData0.push_back(0.0f);

                                                        object.texData.push_back(texes[ti[ei] * 2]);
                                                        object.texData.push_back(-texes[ti[ei] * 2 + 1]);
                                                        object.texData.push_back(1.0f);
                                                        object.texData.push_back(1.0f);

                                                        object.colData.push_back(1.0f);
                                                        object.colData.push_back(1.0f);
                                                        object.colData.push_back(1.0f);
                                                        object.colData.push_back(1.0f);

                                                        object.indexData.push_back(ei);
                                                }
                                        }
                                }
                        }
                }

        }
        else {
                gamePrint("obj file " + path + " not found!\n");
        }

        file.close();
        object.update();
        object.triangleSizeCheck();

        return object;
}

#endif //CORIUM_OBJPARSER_H
