/**
   \brief Contains definitions of public types in graph interface
   
   \author Toni Lammi
*/

#ifndef GRAPHINTERFACETYPES_H
#define GRAPHINTERFACETYPES_H

/**
   \brief Structure for containing graph data
*/
typedef struct{
    int unsigned actPos; //!< Actual motor position
    int unsigned reqPos; //!< Requested motor position
    int unsigned pwmDuty; //!< PWM duty cycle
} GraphData;


#endif
