#ifndef _UTILS_H_
#define _UTILS_H_

namespace PewPew
{
    /// Converts the string to lower case
    /// @param str The string to be lower-cased
    void strToLower(string& str);

    /// Converts a string value to double if possible.
    /// @param s The string with a numerical text (i.e. "99.00", "32.65")
    /// @returns If conversion is successful, the correct value
    ///          is returned. If not, the return value is 0.
    double strToDouble (const string& s);

    /// Parses 3 numbers from a string.
    /// This is used for vector values written on the XML file
    /// like this "0.0, 0.0, 250.0"
    /// @param s The string to parse.
    /// @param val1 The first number will be placed here.
    /// @param val2 The second number will be placed here.
    /// @param val3 The third number will be placed here.
    bool parseDouble3 (const string& s, double& val1, double& val2, double& val3);

    /// Linear interpolation formula
    /// @param t Ratio or time. Range [0.0, 1.0]
    /// @param start The starting value
    /// @param end The final value
    /// @returns The result of the linear interpolation formula
    double  lerp(double t, double start, double end);

    /// Linear interpolation formula applied to vectors
    /// @param t Ratio or time. Range [0.0, 1.0]
    /// @param start The starting point
    /// @param end The final point
    /// @returns The result of the linear interpolation formula
    Vector3 lerpV3(double t, const Vector3& start, const Vector3& end);
}

#endif