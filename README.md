# Спасибо
Акос с линуксовыми системными вызовами у меня был год назад, было очень приятно вспомнить и порешать 

Isachenko Pavel

скомпилировать клиента:
g++ client.cpp -o client

запустить:
./client

как только пришло сообщение от сервера у вас есть возможность ответить использую "y"+Enter далее введите ваше oneline MESSAGE

скомпилировать сервер:
g++ server.cpp -o server 

запустить сервер:
./server

Когда клиент подключится вы увидете его идентификатор. Теперь можно писать "identifier MESSAGE" и клиент с нужным вам идентификатором получит MESSAGE

"666666 MESSAGE" broadcast identifier + MESSAGE

"777777 clients" show clients list 

OS: Linux
