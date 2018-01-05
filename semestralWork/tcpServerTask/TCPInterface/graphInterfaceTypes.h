/**
   \brief Contains definitions of public types in graph interface
*/

#ifndef GRAPHINTERFACETYPES_H
#define GRAPHINTERFACETYPES_H

/**
   \brief Structure for containing graph data
*/
typedef struct{
    int actPos; //!< Actual motor position
    int reqPos; //!< Requested motor position
    int pwmDuty; //!< PWM duty cycle
} GraphData;


#endif
