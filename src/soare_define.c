#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SOARE/SOARE.h>

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
 * SOARE
 * soare_define.c
*/

static Executor executor[0xFF];

AST FunctionModel(char *_Name, AST _Arg, AST _Execute)
{
    AST function = branch(_Name, NODE_FUNCTION);
    AST body = branch("BODY", NODE_BODY);

    if (function == NULL || body == NULL)
    {
        branch_free(function);
        branch_free(body);
        return NULL;
    }

    branch_push(body, _Execute);
    branch_push(function, _Arg);
    branch_push(function, body);

    return function;
}

AST ArgModel(int _Argc, char *_Argv[])
{
    if (_Argv == NULL || _Argc == 0)
        return NULL;
    AST arg = branch(_Argv[0], NODE_GET);
    AST cur = arg;
    for (int _Index = 1; _Index < _Argc; _Index++)
    {
        if (!_Argv[_Index][0])
            break;
        AST tmp = branch(_Argv[_Index], NODE_GET);
        if (tmp == NULL)
            continue;
        cur->_SiblingR = tmp;
        tmp->_SiblingL = cur;
        cur = cur->_SiblingR;
    }
    return arg;
}

void AddCustomExecutor(char *_Name, void (*_Function)(char *))
{
    static unsigned short pos = 0;
    if ((_Name == NULL || _Function == NULL) && pos < 0xFF)
        return;
    executor[pos] = executor[0];
    executor[pos]._Name = _Name;
    executor[pos]._Function = _Function;
    pos++;
}

void RunCustomExecutor(const char *_Name, char *_Arg)
{
    if (_Name == NULL || _Arg == NULL)
        return;
    for (unsigned short _Index = 0; executor[_Index]._Name != NULL; _Index++)
    {
        if (strcmp(executor[_Index]._Name, _Name))
            continue;
        executor[_Index]._Function(_Arg);
        break;
    }
}

AST SoareDefineFunction(AST _Function)
{
    static AST tree = NULL;
    if (tree == NULL)
        tree = branch("SOARE", NODE_FUNCTION);
    branch_push(tree, _Function);
    return tree;
}
