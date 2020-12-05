//
// Created by Roninkoi on 18.12.2015.
//

#include "shader.h"

std::string Shader::readShaderSource(std::string path)
{
	std::string code;

	std::fstream stream(path, std::ios::in);
	if (stream.is_open()) {
		std::string line;
		while (getline(stream, line)) { // awful custom shader macros
			if (line.find("#define MAX_LIGHTS CORIUM_LIGHTNUM") != std::string::npos) {
				code += "\n" + toString("#define MAX_LIGHTS ") + toString(max_lights);
			} else if (line.find("CORIUM_TEXTURE_DEPTHMAP") != std::string::npos) {
				for (int l = 0; l < max_lights; ++l) {
					code += "\n" + toString("if (light_i == ") +
							toString(l) + toString(") closestDepth = texture(depthMap[") +
							toString(l) + toString("], fragToLight).r;\n");
				}
			} else {
				code += "\n" + line;
			}
		}
		stream.close();
	}

	//std::cout << code << std::endl;

	return code;
}

GLuint Shader::loadShaders(std::string vp, std::string fp)
{
	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vsCode = readShaderSource(vp);
	std::string fsCode = readShaderSource(fp);

	GLint result = GL_FALSE;
	int infoLen;

	char const *vsp = vsCode.c_str();
	glShaderSource(vertexShaderID, 1, &vsp, nullptr);
	glCompileShader(vertexShaderID);

	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLen);
	if (infoLen > 0) {
		std::vector<char> VertexShaderErrorMessage(infoLen + 1);
		glGetShaderInfoLog(vertexShaderID, infoLen, nullptr, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	std::cout << "Compiling shader:" << vp << std::endl;
	std::cout << "Compiling shader:" << fp << std::endl;

	char const *fsp = fsCode.c_str();
	glShaderSource(fragmentShaderID, 1, &fsp, nullptr);
	glCompileShader(fragmentShaderID);

	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLen);
	if (infoLen > 0) {
		std::vector<char> FragmentShaderErrorMessage(infoLen + 1);
		glGetShaderInfoLog(fragmentShaderID, infoLen, nullptr, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	GLuint pid = glCreateProgram();
	glAttachShader(pid, vertexShaderID);
	glAttachShader(pid, fragmentShaderID);
	glLinkProgram(pid);

	glGetProgramiv(pid, GL_LINK_STATUS, &result);
	glGetProgramiv(pid, GL_INFO_LOG_LENGTH, &infoLen);
	if (infoLen > 0) {
		std::vector<char> ProgramErrorMessage(infoLen + 1);
		glGetProgramInfoLog(pid, infoLen, nullptr, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(pid, vertexShaderID);
	glDetachShader(pid, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	return pid;
}

GLuint Shader::loadGS(std::string vp, std::string gp, std::string fp)
{
	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER_ARB);
	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vsCode = readShaderSource(vp);
	std::string gsCode = readShaderSource(gp);
	std::string fsCode = readShaderSource(fp);

	std::cout << "Compiling shader:" << vp << std::endl;
	std::cout << "Compiling shader:" << fp << std::endl;
	std::cout << "Compiling shader:" << gp << std::endl;

	GLint Result = GL_FALSE;
	int InfoLogLength;

	char const *VertexSourcePointer = vsCode.c_str();
	glShaderSource(vertexShaderID, 1, &VertexSourcePointer, nullptr);
	glCompileShader(vertexShaderID);

	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(vertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	char const *GeometrySourcePointer = gsCode.c_str();
	glShaderSource(geometryShaderID, 1, &GeometrySourcePointer, nullptr);
	glCompileShader(geometryShaderID);

	glGetShaderiv(geometryShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(geometryShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> GeometryShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(geometryShaderID, InfoLogLength, nullptr, &GeometryShaderErrorMessage[0]);
		printf("%s\n", &GeometryShaderErrorMessage[0]);
	}

	char const *FragmentSourcePointer = fsCode.c_str();
	glShaderSource(fragmentShaderID, 1, &FragmentSourcePointer, nullptr);
	glCompileShader(fragmentShaderID);

	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(fragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, vertexShaderID);
	glAttachShader(ProgramID, geometryShaderID);
	glAttachShader(ProgramID, fragmentShaderID);
	glLinkProgram(ProgramID);

	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, vertexShaderID);
	glDetachShader(ProgramID, geometryShaderID);
	glDetachShader(ProgramID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(geometryShaderID);
	glDeleteShader(fragmentShaderID);

	return ProgramID;
}
