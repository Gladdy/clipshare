import random
import string
import os
import shutil

from django.contrib.auth.models import BaseUserManager
from django.utils import timezone
from django.db.models.manager import BaseManager
from django.db import IntegrityError, models
from clipshare.settings import (STORAGE_DIR,
                                STATIC_DIR,
                                STATIC_URL,
                                SITE_URL)

class UserManager(BaseUserManager):
    def _create_user(self, email, password, is_staff, is_superuser):
        """
        Creates and saves a User with the given username, email and password.
        """
        user = self.model(email=self.normalize_email(email),
                          is_staff=is_staff,
                          is_active=True,
                          is_superuser=is_superuser,
                          date_joined=timezone.now())
        user.set_password(password)
        user.save(using=self._db)
        return user

    def create_user(self, email, password=None):
        return self._create_user(email, password, False, False)

    def create_superuser(self, email, password):
        return self._create_user(email, password, True, True)

def random_id():
    length = 16
    return ''.join(random.choice(string.ascii_letters + string.digits) for i in range(length))


class FileManager(models.Manager):

    def store(self, user, file):

        trial = 0

        while trial < 10:

            identifier = random_id()

            try:
                # Create a database entry for the file
                f = self.model(id=identifier,
                               user=user,
                               filename=file.name,
                               filesize=file.size,
                               mimetype=file.content_type,
                               location="",
                               url="")
                f.save()

                # Actually store the file somewhere on the filesystem
                location = os.path.join(STORAGE_DIR, identifier)
                url = SITE_URL + STATIC_URL + identifier + '/' + file.name

                if not os.path.exists(location): os.makedirs(location)

                with open(os.path.join(location, file.name), 'wb+') as destination:
                    for chunk in file.chunks():
                        destination.write(chunk)


                # Update the database record
                f.location = location
                f.url = url
                f.save()

                return f

            except IntegrityError as e:
                shutil.rmtree(location)
                f.delete()
                print(e)

            trial += 1

        raise IOError("Unable to store the file")