# rrt-tictoc-ibanferro
rrt-tictoc-ibanferro created by GitHub Classroom

Enlace al REPOSITORIO EN GITHUB: https://github.com/ESI-Bilbao/rrt-tictoc-ibanferro.git

Buenas tardes.

Este proyevto ha consistido en emular la red propuesta en el ejercicio utilizando l aherramienta OMNET.

Para realizar este proyecto, se han realizado una serie de abstracciones las cuales se explicarán a continuación.

Uno de los apartados más complicados de la práctica ha sido la creación de conmutadores que redirijan los paquetes a través de la red. En este caso, se trata de un
elementto de la red que se encarga de enrutar los paquetes que recibe a través de sus canales de entrada hacia los canales de salida basándose en un algoritmo de enruta-
miento aleatorio basado en una probabilidad asociada a cada canal de salida. Para simular este comportamiento en OMNET, se han utilizado 2 PARAMETROS de Tipo Cadena de 
Caracteres,los cuales pese a ser cadenas de texto, representan arrays de números enteros. Dado que las distribuciones de probabilidades de este ejercicio eran siempre
quebrados/fracciones, el primer parametro representa la lista de DIVIDENDOS de los numeros fraccionarios del vector de probabilidades, mientras que el otro representa
los DIVISORES del vector de PROBABILIDADES. Adeemás, como en este ejercicio sólo había 2 canales de salida como máximo por cada conmutador, la longitud del vector de 
probabilidades se ha restringido a solo 2 numeros. Es cierto que se podría crear un algoritmo más general que sirviese para más canales de salida, sin embargo, no se 
ha hecho en esta práctica dado que no era del todo necesario para el ejercicio, pero sí que es un punto importante a mejorar en esta práctica.

Otro de los apartados más importantes de esta práctica, ha sido la creación de la clase ProtocolLink, la cual representa un enlace entre 2 conmutadores. Esta clase
ProtocolLink hace uso de la clase DataProtocol para simular la transmisión de un mensaje/paquete a través del enlace. La clase ProtocolLink está conectada a través de 
sus extremos a 2 conmutadores a través de un canal Ideal, es decir, no hay retardo alguno entre el conmutador y la instancia ProtocolLink. Dentro del ProtocolLink,
existen 2 instancias de la clase Transceptor (Transmisor/Receptor) las cuales se transmiten el mensaje/paquete a través del protocolo Stop & Wait. Otro de los puntos a 
mejorar en este proyecto sería implementar otros protocolos como por ejemplo Go Back N, o directamente implementar el caso en el que no hay Prtotocolo alguno y en caso de 
que haya error en la transmisión el paquete se pierda. Cabe destacar que en el DataRateChannel se puede especificar la probabilidad de error de Paquete en el canal.

Por último, se ha creado una clase de Mensaje llamada IbanPaquete, que hereda de la clase cMessage y le añade una serie de campos como por ejemplo, el tiempo en el q se generó dicho paquete, 
el momento en el que el paquete entró al buffer de un ProtocolLink por última vez, el identificador del flujo y el número de saltos realizados por este paquete. 

Muchas gracias por la atención, en caso de tener alguna duda estaré encantado de responderla :)

