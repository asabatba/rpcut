@echo off
color 0F

set "xparser_exe=xpc_inv"

WHERE /Q biserverxmlgen || goto gen_miss_err
WHERE /Q %xparser_exe% || goto xparser_miss_err
WHERE /Q biserverxmlexec || goto exec_miss_err

IF [%1] == [] goto arg_err
IF [%2] == [] goto arg_err
IF [%3] == [] goto arg_err

IF NOT EXIST %~f1 goto rpdin_err


echo Generando XML de entrada...
call biserverxmlgen -P Admin123 -R %~f1 -O .\%~n1.xml -8 || goto xmlgen_err

color 0A

echo Recortando XML...
call %xparser_exe% -i %~n1.xml -o %~n2.xml -a %~3 || goto xparser_err

call start "" "resultados.html" || echo No se ha podido abrir resultados.html

color 0F

echo Generando RPD de salida...
call biserverxmlexec -P Admin123 -I %~n2.xml -O .\%~n2.rpd || goto xmlexec_err


:exit_ok
del %~n1.xml
del %~n2.xml
echo Ok!
exit /b 0

:arg_err
echo Faltan argumentos!
echo Uso del programa:
echo ---
echo --- %0 rpd_entrada.rpd rpd_salida.rpd "OBIAAA ELE"
echo ---
echo Donde OBIAAA, ELE, son las aplicaciones que se quieren conservar del rpd de entrada
goto err

:gen_miss_err
echo No se ha encontrado el ejecutable biserverxmlgen (OBIEE developer tools) en el PATH de Windows
goto err

:exec_miss_err
echo No se ha encontrado el ejecutable biserverxmlexec (OBIEE developer tools) en el PATH de Windows
goto err

:xparser_miss_err
echo No se ha encontrado el ejecutable %xparser_exe% en el directorio
goto err

:rpdin_err
echo No se ha encontrado el RPD de entrada %~f1
goto err

:xparser_err
echo Error quitando las aplicaciones del XML
goto err

:xmlgen_err
echo Error en la generacion del XML (biserverxmlgen)
goto err

:xmlexec_err
echo Error en la generacion del RPD (biserverxmlexec)
goto err

:err
REM echo Error!
exit /b 1