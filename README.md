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


# Результаты

Анализ результатов может быть воспроизведен с помощью `plots_generation.ipynb`

Среднее время работы всех алгоритмов на группе маленьких тестов:

![cpu_small_tests](https://user-images.githubusercontent.com/35899997/119417931-95275f80-bcff-11eb-8a59-d752f0fd9b2f.png)

Среднее время работы алгоритмов (не включая IDA*) на группе больших тестов:

![cpu_large_tests](https://user-images.githubusercontent.com/35899997/119417969-a8d2c600-bcff-11eb-8d1c-579d60bc7687.png)

Производительность PEA* и AnytimeA* на группе маленьких тестов (относительно A*):

![performance_small_tests](https://user-images.githubusercontent.com/35899997/119418087-e6375380-bcff-11eb-8d52-b5114c0d1178.png)

Ресурсы, затрачиваемые PEA* при различных значениях параметра:

![pea_cutoff](https://user-images.githubusercontent.com/35899997/119418225-2c8cb280-bd00-11eb-8056-efea03506ce4.png)

Сходимость границ AnytimeA*:

![anytime_convergence](https://user-images.githubusercontent.com/35899997/119418166-0d8e2080-bd00-11eb-9a86-71a885652590.png)

