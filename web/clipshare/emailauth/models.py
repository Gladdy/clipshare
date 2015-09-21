from django.contrib.auth.models import AbstractBaseUser, PermissionsMixin
from django.db import models

from emailauth.managers import UserManager


class User(AbstractBaseUser, PermissionsMixin):
    """
    Custom user class.
    """
    email = models.EmailField('email address', unique=True, db_index=True)
    is_staff = models.BooleanField(default=False)
    is_active = models.BooleanField(default=True)
    date_joined = models.DateTimeField(auto_now_add=True)

    USERNAME_FIELD = 'email'
    objects = UserManager()

    def __str__(self):
        return self.email

    def get_short_name(self):
        return self.email

    def get_full_name(self):
        return self.email


class UserProfile(models.Model):
    user = models.OneToOneField(User, primary_key=True)