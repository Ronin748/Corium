//
// Created by Roninkoi on 3.8.2016.
//

#ifndef CORIUM_QUADISECT_H
#define CORIUM_QUADISECT_H

#include "corium.h"

bool quadIsect(std::vector<float> q1, std::vector<float> q2)
{
        if (q1.size() != 8 || q2.size() != 8)
                return false;

        float x1 = std::min(q1[0], std::min(q1[2], std::min(q1[4], (q1[6]))));

        float y1 = std::max(q1[1], std::max(q1[3], std::max(q1[5], (q1[7]))));

        float w1 = (float)fabs(std::max(q1[0], std::max(q1[2], std::max(q1[4], (q1[6])))) - x1);

        float h1 = (float)fabs(std::min(q1[1], std::min(q1[3], std::min(q1[5], (q1[7])))) - y1);

        float x2 = std::min(q2[0], std::min(q2[2], std::min(q2[4], (q2[6]))));

        float y2 = std::max(q2[1], std::max(q2[3], std::max(q2[5], (q2[7]))));

        float w2 = (float)fabs(std::max(q2[0], std::max(q2[2], std::max(q2[4], (q2[6])))) - x2);

        float h2 = (float)fabs(std::min(q2[1], std::min(q2[3], std::min(q2[5], (q2[7])))) - y2);

        return (x1 <= x2 + w2) && (x1 + w1 >= x2) &&
               (y1 <= y2 + h2) && (y1 + h1 >= y2) ? true : false;
}

#endif //CORIUM_QUADISECT_H
