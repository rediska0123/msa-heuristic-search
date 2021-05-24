# Алгоритмы эвристического поиска для задачи выравнивания ДНК/РНК

Реализованные алгоритмы:

* Прогрессивное выравнивание

* A*

* IDA*

* [PEA*](https://www.aaai.org/Papers/AAAI/2000/AAAI00-142.pdf)

* [AnytimeA*](https://www.aaai.org/Papers/JAIR/Vol28/JAIR-2808.pdf)

Датасет для тестирования алгоритмов: 

* [BALiBASE4 R1-5](http://www.lbgi.fr/balibase/)

# Запуск алгоритмов на данных

    >> python process_test_data.py  # скачать и предобработать данные
    >> mkdir build && cd build
    >> cmake .. && make all
    >> cd .. && ./build/msa_main    # запустить алгоритмы на данных

Последняя команда сгенерирует два файла:

* `results.csv` : результаты работы алгоритмов на группе больших и маленьких тестов
* `single_test_result.csv` : более подробные результаты для тестов из `data/test_config.txt`. 

Эти файлы также можно скачать [здесь](https://drive.google.com/drive/folders/1urFaWfg3ocT11_Kk40pc_g2cCnSoVl9Y?usp=sharing) (оба файла необходимо положить в корень репозитория).


# Запуск тестов к алгоритмам
    >> mkdir build && cd build
    >> cmake .. && make all
    >> ctest
