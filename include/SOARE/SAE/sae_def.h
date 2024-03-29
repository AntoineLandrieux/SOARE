#ifndef __DEF_FUNCTION_H__
#define __DEF_FUNCTION_H__ 0x1

/*
 _____  _____  ___  ______ _____
/  ___||  _  |/ _ \ | ___ \  ___|
\ `--. | | | / /_\ \| |_/ / |__
 `--. \| | | |  _  ||    /|  __|
/\__/ /\ \_/ / | | || |\ \| |___
\____/  \___/\_| |_/\_| \_\____/

 * Antoine LANDRIEUX
 * GNU General Public License v3.0
 * https://www.gnu.org/licenses/
 * 
 * SOARE HEADER
 * sae_def.h
*/

/**
 * @brief SOARE Executor
 * @author Antoine LANDRIEUX
 */
typedef struct executor
{
    char *_Name;
    void (*_Function)(char *);
} Executor, *EXECUTOR;

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Name
 * @param _Arg
 * @param _Execute
 * @return AST
 */
AST FunctionModel(char *_Name, AST _Arg, AST _Execute);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Argc
 * @param _Argv
 * @return AST
 */
AST ArgModel(int _Argc, char *_Argv[]);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Name
 * @param _Function
 */
void AddCustomExecutor(char *_Name, void (*_Function)(char *));

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Name
 * @param _Arg
 */
void RunCustomExecutor(const char *_Name, char *_Arg);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Function
 * @return AST
 */
AST SoareDefineFunction(AST _Function);

#endif /* __DEF_FUNCTION_H__ */