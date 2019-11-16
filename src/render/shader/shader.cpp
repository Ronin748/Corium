//
// Created by Roninkoi on 18.12.2015.
//

#include "shader.h"

GLuint Shader::loadShaders(const char *vertPath, const char *fragPath, bool src = false) {
        VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

        std::string VertexShaderCode;
        if (!src) {
                std::fstream VertexShaderStream(vertPath, std::ios::in);
                if (VertexShaderStream.is_open()) {
                        std::string Line = "";
                        while (getline(VertexShaderStream, Line))
                                VertexShaderCode += "\n" + Line;
                        VertexShaderStream.close();
                } else {
                        printf("Could not open %s!\n",
                               vertPath);
                        getchar();
                        return 0;
                }
        }
        else {
                VertexShaderCode = vertPath;
        }
        std::string FragmentShaderCode;
        if (!src) {
                std::fstream FragmentShaderStream(fragPath, std::ios::in);
                if (FragmentShaderStream.is_open()) {
                        std::string Line = "";
                        while (getline(FragmentShaderStream, Line)) { // custom shader macros
                                if (Line == "#define MAX_LIGHTS CRM_LIGHTNUM\r" || Line == "#define MAX_LIGHTS CRM_LIGHTNUM\n" || Line == "#define MAX_LIGHTS CRM_LIGHTNUM") {
                                        FragmentShaderCode += "\n" + toString("#define MAX_LIGHTS ") + toString(max_lights);
                                }
                                else if (Line == "CRM_TEXTURE_DEPTHMAP\r" || Line == "CRM_TEXTURE_DEPTHMAP\n" || Line == "CRM_TEXTURE_DEPTHMAP") {
                                        for (int l = 0; l < max_lights; ++l) {
                                                FragmentShaderCode += "\n" + toString("if (light_i == ") + toString(l) + toString(") closestDepth = texture(depthMap[") + toString(l) + toString("], fragToLight).r;\n");
                                        }
                                }
                                else {
                                        FragmentShaderCode += "\n" + Line;
                                }
                        }
                        FragmentShaderStream.close();
                }
        }
        else {
                FragmentShaderCode = fragPath;
        }
        GLint Result = GL_FALSE;
        int InfoLogLength;

        char const *VertexSourcePointer = VertexShaderCode.c_str();
        glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
        glCompileShader(VertexShaderID);

        glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
                std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
                glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
                printf("%s\n", &VertexShaderErrorMessage[0]);
        }

        char const *FragmentSourcePointer = FragmentShaderCode.c_str();
        glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
        glCompileShader(FragmentShaderID);

        glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
                std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
                glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
                printf("%s\n", &FragmentShaderErrorMessage[0]);
        }

        printf("Linking program\n");
        GLuint ProgramID = glCreateProgram();
        glAttachShader(ProgramID, VertexShaderID);
        glAttachShader(ProgramID, FragmentShaderID);
        glLinkProgram(ProgramID);

        glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
                std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
                glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
                printf("%s\n", &ProgramErrorMessage[0]);
        }

        glDetachShader(ProgramID, VertexShaderID);
        glDetachShader(ProgramID, FragmentShaderID);

        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);

        return ProgramID;
}

GLuint Shader::loadGS(const char *vertPath, const char *fragPath, const char *gsPath, bool src = false)
{
        VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER_ARB);
        FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

        std::string VertexShaderCode;

        std::fstream VertexShaderStream(vertPath, std::ios::in);
        if (VertexShaderStream.is_open()) {
                std::string Line = "";
                while (getline(VertexShaderStream, Line))
                        VertexShaderCode += "\n" + Line;
                VertexShaderStream.close();
        } else {
                printf("Could not open %s!\n",
                       vertPath);
                getchar();
                return 0;
        }

        std::string GeometryShaderCode;

        std::fstream GeometryShaderStream(gsPath, std::ios::in);
        if (GeometryShaderStream.is_open()) {
                std::string Line = "";
                while (getline(GeometryShaderStream, Line))
                        GeometryShaderCode += "\n" + Line;
                GeometryShaderStream.close();
        }
        std::string FragmentShaderCode;

        std::fstream FragmentShaderStream(fragPath, std::ios::in);
        if (FragmentShaderStream.is_open()) {
                std::string Line = "";
                while (getline(FragmentShaderStream, Line))
                        FragmentShaderCode += "\n" + Line;
                FragmentShaderStream.close();
        }
        printf("Compiling shader: %s\n", vertPath);
        printf("Compiling shader: %s\n", gsPath);
        printf("Compiling shader: %s\n", fragPath);

        GLint Result = GL_FALSE;
        int InfoLogLength;

        char const *VertexSourcePointer = VertexShaderCode.c_str();
        glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
        glCompileShader(VertexShaderID);

        glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
                std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
                glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
                printf("%s\n", &VertexShaderErrorMessage[0]);
        }

        char const *GeometrySourcePointer = GeometryShaderCode.c_str();
        glShaderSource(GeometryShaderID, 1, &GeometrySourcePointer, NULL);
        glCompileShader(GeometryShaderID);

        glGetShaderiv(GeometryShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(GeometryShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
                std::vector<char> GeometryShaderErrorMessage(InfoLogLength + 1);
                glGetShaderInfoLog(GeometryShaderID, InfoLogLength, NULL, &GeometryShaderErrorMessage[0]);
                printf("%s\n", &GeometryShaderErrorMessage[0]);
        }

        char const *FragmentSourcePointer = FragmentShaderCode.c_str();
        glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
        glCompileShader(FragmentShaderID);

        glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
                std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
                glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
                printf("%s\n", &FragmentShaderErrorMessage[0]);
        }

        printf("Linking program\n");
        GLuint ProgramID = glCreateProgram();
        glAttachShader(ProgramID, VertexShaderID);
        glAttachShader(ProgramID, GeometryShaderID);
        glAttachShader(ProgramID, FragmentShaderID);
        glLinkProgram(ProgramID);

        glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
                std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
                glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
                printf("%s\n", &ProgramErrorMessage[0]);
        }

        glDetachShader(ProgramID, VertexShaderID);
        glDetachShader(ProgramID, GeometryShaderID);
        glDetachShader(ProgramID, FragmentShaderID);

        glDeleteShader(VertexShaderID);
        glDeleteShader(GeometryShaderID);
        glDeleteShader(FragmentShaderID);

        return ProgramID;
}
