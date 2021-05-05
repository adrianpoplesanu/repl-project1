# ran in linux with python 3.8.5, but it can be run on any python3 virtual env
virtualenv sandbox

pip install -r requirements.txt

cd sandbox/src

django-admin startproject app

cd sandbox/src/app
python manage.py runserver

cd sandbox/src/api
uvicorn main:app --reload

Same virtualenvironment used for the django app and the fast-api sources.
Created Dockerfiles for both sources.
